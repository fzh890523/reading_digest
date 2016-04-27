#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>

void print_pwd(struct passwd *pwd);
void print_grp(struct group *grp);

int main()
{
    struct passwd *pwd_uid_1000 = getpwuid(1000);
    printf("pwd from uid 1000:\n");
    print_pwd(pwd_uid_1000);

    struct passwd *pwd_uname_yonka = getpwnam("yonka");
    printf("pwd from uname yonka:\n");
    print_pwd(pwd_uname_yonka);

    struct passwd *pwd_uname_vivian = getpwnam("vivian");
    printf("pwd from uname vivian:\n");
    print_pwd(pwd_uname_vivian);
}

void print_pwd(struct passwd *pwd)
{
  printf("pw_name: %s, pw_passwd: %s, pw_uid: %d, pw_gid: %d, pw_gecos: %s, pw_dir: %s, pw_shell: %s\n",
    pwd->pw_name, pwd->pw_passwd, pwd->pw_uid, pwd->pw_gid, pwd->pw_gecos, pwd->pw_dir, pwd->pw_shell);
}

void print_grp(struct group *grp)
{
  printf("gr_name: %s, gr_passwd: %s, gr_gid: %d, gr_mem...\n", grp->gr_name, grp->gr_passwd, grp->gr_gid);
}
