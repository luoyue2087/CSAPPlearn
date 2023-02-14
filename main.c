void gets(char*);
void puts(char*);
void echo(){
    char buf[8];
    gets(buf);
    puts(buf);
}
//g++ -S main.c -Og
//g++ -o prog main.c