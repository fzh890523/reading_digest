import sys
from numbers import Number


class Stack(object):
    def __init__(self):
        self._l = []

    def push(self, item):
        self._l.append(item)

    def pop(self):
        self._l.pop(0)


class Converter(object):
    """
    a+b*c+(d*e+f)*g -> abc*+de*f+g*
    a+b*c+g*(d*e+f) -> abc*+de*f+g*
    """

    priority_ops = {
        10: ['('],
        9: ['*', '/', '%'],
        8: ['+', '-'],
        7: [')']
    }
    op_priorities = {}
    for k, v in priority_ops.items():
        for op in v:
            op_priorities[v] = k

    def __init__(self, expr=None, lexes=None):
        if lexes is None:
            if expr is None:
                raise ValueError("expr and lexes can not be both none")
            else:
                lexes = self.lex(expr)
        self._expr = expr
        self._lexes = lexes
        self._values = Stack()
        self._ops = Stack()
        self._last_v = None
        self._last_op = None
        self._last_l = None
        self._iterated = False
        self._ret = Stack()

    def lex(self, expr):
        pass

    @classmethod
    def is_value(cls, l):
        return isinstance(l, Number)

    @classmethod
    def is_op(cls, l):
        return isinstance(l, str)

    @classmethod
    def is_bracket(cls, l):
        return cls.is_op(l) and (l == '(' or l == ')')

    @classmethod
    def op_priority(cls, op):
        return cls.op_priorities[op]

    @classmethod
    def cmp_op(cls, op1, op2):
        v1, v2 = cls.op_priority(op1), cls.op_priority(op2)
        d = v1 - v2
        return d if d == 0 else d / abs(d)

    def pop_and_eval(self):
        self._ret.push(self._values.pop())
        self._ret.push(self._values.pop())
        self._ret.push(self._ops.pop())

    def convert(self):
        for l in self._lexes:
            if self.is_value(l):
                if self.is_value(self._last_l):
                    raise ValueError("sequent value: %s - %s" % (l, self._last_l))
                self._values.push(l)
            elif self.is_op(l):
                if self.is_op(self._last_l):
                    if (not self.is_bracket(l)) and (not self.is_bracket(self._last_l)):
                        raise ValueError("sequent non-bracket op: %s - %s", l, self._last_l)
                if l == '(':
                    self.last_l = None
                elif l == ')':
                    self.pop_and_eval()
                else:
                    if self._last_op is not None and self.cmp_op(l, self._last_op) <= 0:
                        self.pop_and_eval()
                    self._ops.push(l)

        self._iterated = True
        self.pop_and_eval()


if __name__ == "__main__":
    Converter
