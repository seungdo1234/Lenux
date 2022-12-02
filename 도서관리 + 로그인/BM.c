#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include "Book.h"
#include "member.h"
#include <time.h>
#include <wait.h>

void printLogin()
{
    printf("1. 일반회원 Login\n");
    printf("2. Admin 회원 Login\n");
    printf("3. 신규회원 가입\n");
    printf("0. Quit\n");
    printf("Choose Num  >>>    ");
}
void fileLock(char fileName[], struct Book record, struct flock lock, int fd, int search, int lockType)
{
    if (lockType == 1)
        lock.l_type = F_RDLCK;
    else
        lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = search * sizeof(record);
    if (search == 0)
        lock.l_len = 0;
    else
        lock.l_len = sizeof(record);
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror(fileName);
        exit(2);
    }
}
void memberFileLock(char memberDataFile[], struct member memberRecord, struct flock lock, int fd, int search, int lockType)
{
    if (lockType == 1)
        lock.l_type = F_RDLCK;
    else
        lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = search * sizeof(memberRecord);
    if (search == 0)
        lock.l_len = 0;
    else
        lock.l_len = sizeof(memberRecord);
    if (fcntl(fd, F_SETLKW, &lock) == -1)
    {
        perror(memberDataFile);
        exit(2);
    }
}
int login(struct member memberRecord, char fileName[], int sel, struct flock lock)
{
    char loginID[100], loginPW[100], count = 1;
    int fd = open(fileName, O_RDONLY);
    while (count != 4)
    {
        if(sel == 1) printf("<1. 일반회원 Login>\n");
        if(sel == 2) printf("<1. Admin 회원 Login>\n");
        printf("id와 password를 입력하세요.\n");
        printf("id : ");
        scanf("%s", loginID);
        printf("password : ");
        scanf("%s", loginPW);
        for (int i = 1;; i++)
        {
            memberFileLock(fileName, memberRecord, lock, fd, i, 1);
            lseek(fd, i * sizeof(memberRecord), SEEK_SET);
            if (read(fd, (char *)&memberRecord, sizeof(memberRecord)) != sizeof(memberRecord))
                break;
            if (strcmp(memberRecord.memberID, loginID) == 0 && strcmp(memberRecord.memberPW, loginPW) == 0)
            {
                if (memberRecord.member_Num > 0 && (sel == 1 && memberRecord.isAdmin == 'N') || (sel == 2 && memberRecord.isAdmin == 'Y'))
                {
                    printf("로그인 성공 ! \n\n");
                    close(fd);
                    if (sel == 1)
                        return 1;
                    if (sel == 2)
                        return 2;
                }
            }
            lock.l_type = F_ULOCK;
            fcntl(fd,F_SETLK,&lock);
        }
        printf("로그인 실패 ! (%d/3)\n\n", count);
        count++;
    }
    close(fd);
    return 0;
}
void memberRegister(struct member memberRecord, char fileName[], struct flock lock)
{
    char cloneID[100];
    int count = 1;
    printf("<< 신규 회원가입 >>\n");
    printf("id : ");
    scanf("%s", cloneID);
    int fd = open(fileName, O_RDWR);
    for (int i = 1;; i++)
    {
        memberFileLock(fileName, memberRecord, lock, fd, i, 1);
        lseek(fd, i * sizeof(memberRecord), SEEK_SET);
        if (read(fd, (char *)&memberRecord, sizeof(memberRecord)) != sizeof(memberRecord))
            break;
        if (memberRecord.member_Num > 0 && strcmp(memberRecord.memberID, cloneID) == 0)
        {
            printf("이미 존재하는 ID 입니다 !!\n\n");
            close(fd);
            return;
        }
        count++;
        lock.l_type = F_ULOCK;
        fcntl(fd,F_SETLK,&lock);
    }
    memberFileLock(fileName, memberRecord, lock, fd, memberRecord.member_Num, 2);
    memberRecord.member_Num = count;
    strcpy(memberRecord.memberID, cloneID);
    printf("pass : ");
    scanf("%s", memberRecord.memberPW);
    printf("회원 이름 : ");
    scanf("%s", memberRecord.memberName);
    printf("휴대폰 번호 : ");
    scanf("%s", memberRecord.memberPhoneNumber);
    printf("이메일 : ");
    scanf("%s", memberRecord.memberEmail);
    printf("생년월일 (YYMMDD) : ");
    scanf("%s", memberRecord.memberBirthday);
    printf("admin 여부 (Y/N) : ");
    scanf(" %c", &memberRecord.isAdmin);
    lseek(fd, count * sizeof(memberRecord), SEEK_SET);
    write(fd, (char *)&memberRecord, sizeof(memberRecord));
    lock.l_type = F_ULOCK;
    fcntl(fd, F_SETLK, &lock);
}
int loginMenu(char fileName[])
{
    int sel = -1, isLogin = 0;
    struct member memberRecord;
    struct flock lock;
    while (1)
    {
        printf("BooK Management Service\n");
        printLogin();
        scanf("%d", &sel);
        if (sel == 1 || sel == 2)
            isLogin = login(memberRecord, fileName, sel, lock);
        else if (sel == 3)
            memberRegister(memberRecord, fileName, lock);
        else if (sel == 0)
            exit(0);
        if (isLogin == 1 || isLogin == 2)
            return isLogin;
    }
}
void printMenu(int isLogin)
{
    printf("Book Management\n");
    printf("1. List up All Book (Sort by 식별자)\n");
    printf("2. List up All Book (Sort by 도서명)\n");
    printf("3. Add a New Book\n");
    printf("4. Update a New Book\n");
    printf("5. Remove a Book\n");
    printf("6. Search a Book Information by Title (minimum 2char)\n");
    if (isLogin == 1)
        printf("7. Search a Book Information by ID\n");
    else if (isLogin == 2)
    {
        printf("7. 회원리스트 보기\n");
        printf("8. 회원정보 삭제\n");
        printf("9. 회원정보 갱신\n");
    }
    printf("0. Quit\n");
    printf("Choose Num -->   ");
}
void printBookList(BK temp)
{
    printf("\n1. 도서번호 : %d\n", temp.book_Num);
    printf("2. 도서명 : %s \n", temp.book_Name);
    printf("3. 저자명 : %s \n", temp.writer);
    printf("4. 출판년월일 : %s \n", temp.publishing);
    printf("4. 가격 : %d \n", temp.price);
    printf("5. 추천리뷰 : %s \n\n", temp.review);
}
void printMemberList(MB temp)
{
    printf("\nNum : %d\n", temp.member_Num);
    printf("ID : %s\n", temp.memberID);
    printf("PW : : %s \n", temp.memberPW);
    printf("Name :  %s \n", temp.memberName);
    printf("PhoneNum : %s \n", temp.memberPhoneNumber);
    printf("Email : %s \n", temp.memberEmail);
    printf("Brithday : %s \n", temp.memberBirthday);
    printf("Admin : %c \n\n", temp.isAdmin);
}
int dbLength(char fileName[], struct Book record, struct flock lock)
{
    int fd = open(fileName, O_RDONLY), count = 0;
    for (int i = 1;; i++)
    {
        if (i != 1)
            fileLock(fileName, record, lock, fd, i - 1, 1);
        if (read(fd, (char *)&record, sizeof(record)) != sizeof(record))
            break;
        if (record.book_Num > 0)
        {
            count++;
            ;
        }
        lock.l_type = F_ULOCK;
        fcntl(fd, F_SETLK, &lock);
        lseek(fd, i * sizeof(record), SEEK_SET);
    }
    return count;
    close(fd);
}
void listUpID(char fileName[], struct Book record, struct flock lock)
{
    int fd = open(fileName, O_RDONLY);
    for (int i = 1;; i++)
    {
        fileLock(fileName, record, lock, fd, i, 1);
        lseek(fd, i * sizeof(record), SEEK_SET);
        if (read(fd, (char *)&record, sizeof(record)) != sizeof(record))
            break;
        if (record.book_Num > 0)
        {
            printBookList(record);
        }
        lock.l_type = F_ULOCK;
        fcntl(fd, F_SETLK, &lock);
    }
    close(fd);
}
int cmp(const void *a, const void *b)
{
    return strcmp(((BK *)a)->book_Name, ((BK *)b)->book_Name);
}
void listUpName(char fileName[], struct Book record, struct flock lock)
{
    int fd = open(fileName, O_RDONLY);
    fileLock(fileName, record, lock, fd, 0, 1);
    lock.l_type = F_ULOCK;
    fcntl(fd, F_SETLK, &lock);
    int count = dbLength(fileName, record, lock);
    BK *temp = (BK *)malloc(sizeof(BK) * count);
    int num = 0;
    for (int i = 1;; i++)
    {
        if (read(fd, (char *)&record, sizeof(record)) != sizeof(record))
            break;
        if (record.book_Num > 0)
        {
            temp[num] = record;
            num++;
        }
        lseek(fd, i * sizeof(record), SEEK_SET);
    }
    qsort(temp, count, sizeof(BK), cmp);
    for (int i = 0; i < count; i++)
        printBookList(temp[i]);
    free(temp);
    close(fd);
}
void addData(char fileName[], struct Book record, struct flock lock)
{
    int search = 0;
    char sel;
    printf("추가 할 도서정보의 도서번호를 입력하시오 (1 이상)   ");
    scanf("%d", &search);
    if (search < 1)
    {
        printf("Error ... (도서번호 1이상) \n\n");
        return;
    }
    int fd = open(fileName, O_RDWR);
    lseek(fd, search * sizeof(record), SEEK_SET);
    read(fd, (char *)&record, sizeof(record));
    if (record.book_Num > 0)
    {
        printf("Error ... (중복 데이터)\n\n");
        close(fd);
        return;
    }
    record.book_Num = search;
    fileLock(fileName, record, lock, fd, record.book_Num, 2);
    printf("도서명, 저자명, 출판년월일, 가격, 추천리뷰(공백포함) \n");
    scanf("%s %s %s %d %[^\n]s", record.book_Name, record.writer, record.publishing, &record.price, record.review);
    lseek(fd, record.book_Num * sizeof(record), SEEK_SET);
    write(fd, (char *)&record, sizeof(record));
    lock.l_type = F_ULOCK;
    fcntl(fd, F_SETLK, &lock);
    record.book_Num = 0;
    close(fd);
    printf("데이터를 추가적으로 입력하시겠습니까 ?  (Y N)  ");
    scanf(" %c", &sel);
    if (sel == 'Y')
        addData(fileName, record, lock);
}
void updateData(char fileName[], struct Book record, struct flock lock)
{
    int search = 0;
    printf("수정할 도서정보의 도서번호를 입력하시오  ");
    scanf("%d", &search);
    int fd = open(fileName, O_RDWR);
    lseek(fd, search * sizeof(record), SEEK_SET);
    read(fd, (char *)&record, sizeof(record));
    if (record.book_Num > 0)
    {
        fileLock(fileName, record, lock, fd, record.book_Num, 2);
        printf("도서명, 저자명, 출판년월일, 가격, 추천리뷰(공백포함) \n");
        scanf("%s %s %s %d %[^\n]s", record.book_Name, record.writer, record.publishing, &record.price, record.review);
        lseek(fd, record.book_Num * sizeof(record), SEEK_SET);
        write(fd, (char *)&record, sizeof(record));
        lock.l_type = F_ULOCK;
        fcntl(fd, F_SETLK, &lock);
    }
    else
        printf("\n해당 도서정보가 존재하지 않습니다. \n\n");
    close(fd);
}
void removeData(char fileName[], struct Book record, struct flock lock)
{
    int search = 0;
    printf("삭제할 도서정보의 도서번호를 입력하시오  ");
    scanf("%d", &search);
    int fd = open(fileName, O_RDWR);

    lseek(fd, search * sizeof(record), SEEK_SET);
    read(fd, (char *)&record, sizeof(record));
    if (record.book_Num > 0)
    {
        fileLock(fileName, record, lock, fd, record.book_Num, 2);
        record.book_Num = -1;
        lseek(fd, search * sizeof(record), SEEK_SET);
        write(fd, (char *)&record, sizeof(record));
        lock.l_type = F_ULOCK;
        fcntl(fd, F_SETLK, &lock);
        printf("%d번 삭제완료 !!! \n\n", search);
    }
    else
        printf("\n해당 도서정보가 존재하지 않습니다. \n\n");
    close(fd);
}
void searchDataTitle(char fileName[], struct Book record, struct flock lock)
{
    int fd = open(fileName, O_RDONLY);
    char search[50];
    printf("검색할 도서정보의 도서명을 입력하시오 (최소 2자) ");
    scanf("%s", search);
    for (int i = 1;; i++)
    {
        fileLock(fileName, record, lock, fd, i, 1);
        lseek(fd, i * sizeof(record), SEEK_SET);
        if (read(fd, (char *)&record, sizeof(record)) != sizeof(record))
            break;
        if (record.book_Num > 0 && strstr(record.book_Name, search))
        {
            printBookList(record);
        }
        lock.l_type = F_ULOCK;
        fcntl(fd, F_SETLK, &lock);
    }
    close(fd);
}
void searchDataKey(char fileName[], struct Book record, struct flock lock)
{
    int search = 0;
    printf("검색할 도서정보의 도서번호를 입력하시오  ");
    scanf("%d", &search);
    int fd = open(fileName, O_RDONLY);
    fileLock(fileName, record, lock, fd, search, 1);
    lseek(fd, search * sizeof(record), SEEK_SET);
    read(fd, (char *)&record, sizeof(record));
    lock.l_type = F_ULOCK;
    fcntl(fd, F_SETLK, &lock);
    if (record.book_Num > 0)
    {
        printBookList(record);
    }
    else
        printf("\n해당 도서정보가 존재하지 않습니다. \n\n");
    close(fd);
}
void memberListUp(char memberDataFile[], struct member memberRecord, struct flock lock)
{
    int fd = open(memberDataFile, O_RDONLY);
    for (int i = 1;; i++)
    {
        memberFileLock(memberDataFile, memberRecord, lock, fd, i, 1);
        lseek(fd, i * sizeof(memberRecord), SEEK_SET);
        if (read(fd, (char *)&memberRecord, sizeof(memberRecord)) != sizeof(memberRecord))
            break;
        if (memberRecord.member_Num > 0)
        {
            printMemberList(memberRecord);
        }
        lock.l_type = F_ULOCK;
        fcntl(fd, F_SETLK, &lock);
    }
    close(fd);
}
void removeMemberData(char memberDataFile[], struct member memberRecord, struct flock lock)
{
    int search = 0;
    printf("삭제할 멤버의 번호를 입력하시오  ");
    scanf("%d", &search);
    int fd = open(memberDataFile, O_RDWR);

    lseek(fd, search * sizeof(memberRecord), SEEK_SET);
    read(fd, (char *)&memberRecord, sizeof(memberRecord));
    if (memberRecord.member_Num > 0)
    {
        memberFileLock(memberDataFile, memberRecord, lock, fd, memberRecord.member_Num, 2);
        memberRecord.member_Num = -1;
        lseek(fd, search * sizeof(memberRecord), SEEK_SET);
        write(fd, (char *)&memberRecord, sizeof(memberRecord));
        lock.l_type = F_ULOCK;
        fcntl(fd, F_SETLK, &lock);
        printf("%d번 멤버 삭제완료 !!! \n\n", search);
    }
    else
        printf("\n해당 멤버정보가 존재하지 않습니다. \n\n");
    close(fd);
}
void updateMemberData(char memberDataFile[], struct member memberRecord, struct flock lock)
{
    int search = 0;
    printf("수정할 멤버의 번호를 입력하시오  ");
    scanf("%d", &search);
    int fd = open(memberDataFile, O_RDWR);

    lseek(fd, search * sizeof(memberRecord), SEEK_SET);
    read(fd, (char *)&memberRecord, sizeof(memberRecord));
    if (memberRecord.member_Num > 0)
    {
        memberFileLock(memberDataFile, memberRecord, lock, fd, memberRecord.member_Num, 2);
        printf("회원 이름 : ");
        scanf("%s", memberRecord.memberName);
        printf("휴대폰 번호 : ");
        scanf("%s", memberRecord.memberPhoneNumber);
        printf("이메일 : ");
        scanf("%s", memberRecord.memberEmail);
        printf("생년월일 (YYMMDD) : ");
        scanf("%s", memberRecord.memberBirthday);
        printf("admin 여부 (Y/N) : ");
        scanf(" %c", &memberRecord.isAdmin);
        lseek(fd, search * sizeof(memberRecord), SEEK_SET);
        write(fd, (char *)&memberRecord, sizeof(memberRecord));
        lock.l_type = F_ULOCK;
        fcntl(fd, F_SETLK, &lock);
    }
    else
        printf("\n해당 멤버정보가 존재하지 않습니다. \n\n");
    close(fd);
}
void bookMainLoop(char memberDataFile[], int isLogin, char fileName[])
{
    struct Book record;
    struct member memberRecord;
    struct flock lock;
    int sel = -1;
    int fd = open(fileName,O_RDWR | O_CREAT, 0660);
    close(fd);
    while (sel != 0)
    {
        printMenu(isLogin);
        scanf("%d", &sel);
        if (sel == 1)
            listUpID(fileName, record, lock);
        if (sel == 2)
            listUpName(fileName, record, lock);
        if (sel == 3)
            addData(fileName, record, lock);
        if (sel == 4)
            updateData(fileName, record, lock);
        if (sel == 5)
            removeData(fileName, record, lock);
        if (sel == 6)
            searchDataTitle(fileName, record, lock);
        if (sel == 7 && isLogin == 1)
            searchDataKey(fileName, record, lock);
        if (sel == 7 && isLogin == 2)
            memberListUp(memberDataFile, memberRecord, lock);
        if (sel == 8 && isLogin == 2)
            removeMemberData(memberDataFile, memberRecord, lock);
        if (sel == 9 && isLogin == 2)
            updateMemberData(memberDataFile, memberRecord, lock);
    }
}
void loop(char fileName[])
{
    int pid = fork(), status;
    if(pid == 0){
        int isLogin = loginMenu(fileName);
        bookMainLoop(fileName, isLogin, "Book_wltmdeh1801215.dbx");
        exit(0);
    }
    else{
       wait(&status);
    }
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "사용법 : ./%s file", argv[0]);
        exit(2);
    }
    int fd = open(argv[1], O_RDWR | O_CREAT, 0660);
    if (fd == -1)
    {
        close(fd);
        perror(argv[1]);
        exit(2);
    }
    close(fd);
    loop(argv[1]);
}