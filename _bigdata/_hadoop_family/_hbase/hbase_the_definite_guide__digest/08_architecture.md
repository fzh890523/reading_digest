

# 数据查找和传输

## B+树

**应用**
> 没有太多的修改（写操作）
>> 为了保证处理时间，通常都是随机写

## LSM树

### 介绍


### 原理

#### 写操作
> 背景
1. 根据row_key查到应该存到哪个region，以及这个region的server

1. 先写入log文件，其中数据完全有序（应该是按写入时间）
2. 内存会和log文件对应，写log文件后更新内存
3. 写了很多次以后接近配置的大小，会把日志中内容写磁盘（实际存储），同时创建一个新的数据存储文件，然后清空内存里的数据
4. 多次刷写得到的多个数据存储文件，后台线程会进行聚合（成少数大数据文件），这样的话查找就可以在少数文件中进行

#### 读操作
1. 查内存（前面提到，内存里总是最新的（如果内存里有的话））
2. 1没查到则查磁盘文件

#### 删除操作
> 特殊的修改，标记删除。 页重写时会丢弃

### 实现
...

### 特点
> 日志文件+内存存储的机制 将 随机写 转换成 顺序写。 所以能保证稳定的数据写入速率；
> 读写独立，互相之间没有冲突
> **【连续存储】** 存储数据的布局较好，查询一个key的磁盘寻道次数在一个可预测的范围内，并且读取与该key连续的任意数量的记录都不需要额外寻道
> 基于前一条： 做到 **cost（开销）透明**（作为卖点） - 一次请求需要的cost相对稳定，没有较大抖动

**hbase和bigtable都基于LSM树**

## 比较
> B树和LSM树最主要的区别是“如何利用硬件，特别是硬盘”


# 存储

## hbase中的文件
* 预写日志WAL文件
* 实际存储的数据文件

由regionserver从管理，特殊情况下hmaster也可以进行一些底层文件操作（看hbase版本）

## regionserver对文件的管理
HRegionServer： -包含- HRegion实例（可能多个，每个对应到一个region： 某table的某个region）
HRegion：-包含- Store实例（可能多个，每个对应到一个表的一个family - HColumnFamily）
Store：-包含-  StroeFile实例（可能多个，对实际存储文件HFile的浅封装）
Store - 每个对应一个 MemStore
MemStore： ...

## 写路径
1. 是否写WAL（Put请求可以指定不写WAL，但一般不建议）
  1. 写WAL
2. 写MemStore
...

## 文件
使用的HDFS，默认root dir是/hbase，如果多hbase集群共用一个hdfs的情况，需要自行指定root dir。
用 dfs -lsr /hbase 的方式可以看hbase写入的文件。

文件可以分为几类：
* hbase根目录下 - 根级文件
> Hlog实例管理的**WAL**文件。因为日志滚动，所以会有多个。
> **一个regionserver的所有region共享一组Hlog文件**
* hbase根目录中的表目录下 - 表级文件
* 表目录中的region目录下 - region级文件

### 文件操作

#### region拆分

#### 合并

### HFile文件格式

# WAL
> 积攒足够多的数据再刷写，以避免创建很多小文件
> 但只是内存的话又不稳定可能丢数据
> 所以用“顺序写”的文件来作为这个buf的内容部分的支撑

## Hlog类
> 一台regionserver上多个region（甚至不同table）共享同一个Hlog，意味着多个region的日志会按到达顺序写入WAL。 这在日志回放场景会带来一些额外的工作，不过考虑到这很少发生，所以还是优先考虑普通场景的性能...
>> bigtable论文里提到per-region的话会有很多并发的文件写入，而考虑到底层DFS的实现，会导致大量的硬盘寻道... - 虽然底层FS可换，但“考虑能考虑的，做能做的...”，毕竟以HDFS为主

## HLogKey类
KeyValue： rowkey、family、qualifier、timestamp、类型、value
HLogKey：序列号、region、表名，还记录了写入日志的时间戳...。 这个类存储了多个集群之间进行复制需要的cluster ID

## WALEdit类
client发来的每个修改 -封装-> WALEdit实例。
通过日志级别来管理原子性。
> `更新了一行中的10列` -> `10个KeyValue实例（cell）`。 如果写入5个到WAL后失败了，那么会出现一行更新了一半的情况
> `把多个cell的update` -打包到-> `单个WALEdit实例`,就可以在一次操作中写入 - 以保证行原子性。

## LogSyncer类
WAL的对于HDFS的多datanode（副本）写入。
每一次编辑都需要sync。
sync的代价高，可以选择稍微延迟执行，但注意风险。
需要3个datanode都确认了写操作才算写入成功，有两种方式：
* pipeline 管道/流水线
> regionserver - datanode1 - datanode2 - datanode3
> 好处： 节约regionserver带宽
> 坏处： 时延
* n-way 多路
> regionserver - datanode1/2/3
> 好处坏处和pipeline相反

### deferred log flush
为true的话会使得修改缓存在regionserver，LogSyncer类会用线程去（很短的时间间隔的）调用sync（默认1s，可配置）。
只用于用户表，catalog目录表永远会立即sync。

## LogRoller
滚日志
默认一小时滚一次。 HLog.rollWriter() - HLog.cleanOldLogs()
还有一些触发方式： 大小（默认32MB）、比例（默认95%）...

## 回放
前面提到WAL的“单份”特点，回放时需要把日志分成合适的片... - 所以master没办法把一个崩溃的服务器上的region立即部署到其他服务器，要等待对应region的日志被拆分出来
如果regionserver崩溃的时机是接近要把WAL刷写时的话，此时WAL的量会比较大。

### 日志拆分


### 数据恢复

## 持久性


# 读路径
一个简单的get请求，也是需要scan的，因为多列之间并不一定顺序存储，可能横跨任意数目个存储文件（#yonka# 各版本实现有变化没？）

## QueryMatcher、ColumnTracker
前者精确的匹配出拥护要取的列、后者包含所有的列。 都可以设定最大匹配的版本数。
HBase中没有直接访问特定行或者列的索引文件，HFile中最小的单元是块。 为了找到所需要的数据，RS（Store实例）必须载入整个可能存储着所需数据的块并且扫描这个块。
每个Store实例对应一个列族。如果读操作忽略了某些列族的话，那么可以跳过对应的文件的扫描。



# region查找

## 目录表
* -ROOT-
> 用于查询.META.表中region的位置
> 只有一个root region，不拆分
* .META.

## 查找过程
1. 从zk中获取root region位置信息节点
如：
> /hbase/root-region-server -> "RegionServer R"
2. -ROOT-中查到对应meta region的位置（也即某 table-key）
> tableN的meta信息 -> RegionServer M<sub>n</sub>
3. 从 .META. 表查找用户表对应的region的位置
> tableN的row-189起始行 -> RegionServer U<sub>m</sub>
4. 从...获取到数据

# region生命周期
* offline - region下线
* pending open - 打开region的请求已经发送到了服务器
* opending - 开始打开region
* open - 已经打开，完全可以使用
* pending close - 关闭region的请求被发送到服务器
* closing - 正确处理要关闭的region
* closed - 被关闭
* splitting - 开始切分region
* split - region已经被切分了

状态改变可能由master或RS发起。
拆分过程由RS发起。
用ZK来跟踪znode状态。

# zookeeper
/hbase根目录，可以由 zookeeper.znode.parent 属性设置

/hbase/hbaseid
> 包含cluster ID，和存储在HDFS上的hbase.id文件内容相同
> /hbase/hbaseid:...

/hbase/master
> master server的机器名
> /hbase/master:...

/hbase/replication
> 副本信息 - 后面节

/hbase/root-region-server
> -ROOT- region所在RS的机器名
> /hbase/root-region-server:...

/hbase/rs
> 所有region服务器的根节点... 都是临时节点，node名仕region服务器名
> /hbase/rs/[..., ...]

/hbase/shutdown
> 跟踪鸡群状态信息，包括启动时间、被关闭时的空状态
> /hbase/shutdown:...

/hbase/splitlog
> 协调日志拆分
> /hbase/splitlog/[....]

/hbase/table
> 记录表禁用信息
> /hbase/table/[{tablename:DISABLED}]

/hbase/unassigned
> assignmentmanager用来跟踪集群的region状态，包含未打开的region的znode
> /hbase/unassigned/[{region_hash}]

# 复制
> 在不同的Hbase部署（集群）中复制数据。

hbase复制的基本架构模式是： 主推送master-push。
每个RS都有自己的WAL/HLog，可以... 每个RS都会参与复制自己的修改。

复制异步进行 - 可以地理上原理（对时延不敏感） - 最终一致性
复制格式跟mysql的基于语句的概念相同，不过所有的WALEdits都会被复制以保证原子性。

主集群会用ZK来记录当前WAL中被记录的位置。

## 复制的内部机制和操作

1. 挑选要复制的目标服务器
> zk的/hbase/rc发现接收复制流的服务器，随机挑选一部分服务器（默认10%）来复制数据（会有failover，如果一台在复制时无响应超过容忍度则会更换）
> 每次复制随机挑选，使得负荷尽量平均

2. 跟踪日志中被复制到的位置
> 每个主集群的RS都在 znode 目录有对应的znode，然后在从集群也有对应的znode（多少个从集群则有多少（倍）个） ，每个znode都白喊一个需要处理的HLog对象，跟踪region服务器创建的HLog

3. 读取、过滤以及发送数据修改
每

4. 清理日志

5. region服务器异常
