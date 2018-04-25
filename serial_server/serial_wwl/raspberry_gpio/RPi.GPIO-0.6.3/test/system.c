#include <stdio.h>
#include <stdio.h>
int main()
{
    printf("system cmd test\n");
    char *cmd1 = "touch hahaha";
    system(cmd1);
    //点亮LED
    char*cmd2 = "python ../github_work/RPi.GPIO-0.6.3/test/test1.py";
    system(cmd2);

    return 0;
}
