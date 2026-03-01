#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//学生结构体
typedef struct Student {
    int id; // 学号
    char name[50]; // 姓名
    char gender; // 性别 M/W
    int age; // 年龄
    float score; // 成绩
    struct Student *next;
} Student;

//用户结构体
typedef struct User {
    char userName[50];
    char password[50];
    char role[20]; // 角色 student/teacher/admin
} User;

//申诉结构体
typedef struct Appeal {
    int appealId; // 申诉ID
    char studentName[50]; // 申诉学生
    int studentId; // 学生学号
    float originalScore; // 原成绩
    float requestedScore; // 申请的成绩
    char reason[200]; // 申诉理由
    char status[20]; // 状态：待处理
    struct Appeal *next;
} Appeal;

int position = 0;
Student *head = NULL;
int studentCount = 0; // 学生总数
char currentUser[50] = ""; // 登录用户名称
char currentRole[20] = ""; // 用户角色
Appeal *appealHead = NULL;
int appealCount = 0;
int nextAppealId = 1; //申诉记录的id

// 清空输入缓冲区
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
    return;
}

//保存申诉到文件
void saveAppealsToFile() {
    FILE *fp = fopen("appeals.dat", "wb");
    if (fp == NULL) {
        printf("无法打开文件\n");
        return;
    }
    Appeal *current = appealHead;
    while (current != NULL) {
        fwrite(current, sizeof(Appeal), 1, fp);
        current = current->next;
    }
    fclose(fp);
}

// 从文件加载申诉
void loadAppealsFromFile() {
    FILE *fp = fopen("appeals.dat", "rb");
    if (fp == NULL) {
        printf("打开文件失败");
        return;
    }
    Appeal *current = appealHead; //清空原链表
    while (current != NULL) {
        Appeal *temp = current;
        current = current->next;
        free(temp);
    }
    appealHead = NULL;
    appealCount = 0;
    Appeal temp;
    int maxId = 0;
    Appeal *last = NULL;
    while (fread(&temp, sizeof(Appeal), 1, fp) == 1) {
        Appeal *newNode = (Appeal *) malloc(sizeof(Appeal));
        if (newNode == NULL) {
            printf("内存分配失败\n");
            break;
        }
        *newNode = temp;
        newNode->next = NULL;
        if (appealHead == NULL) {
            appealHead = newNode;
            last = appealHead;
        } else {
            last->next = newNode;
            last = last->next;
        }
        appealCount++;
        if (temp.appealId > maxId) {
            maxId = temp.appealId;
        }
    }
    nextAppealId = maxId + 1;
    fclose(fp);
}

// 学生发起申诉
void studentAppeal() {
    if (strcmp(currentRole, "student") != 0) {
        printf("只有学生可以发起申诉\n");
        return;
    }
    printf("发起成绩申诉 \n");
    Appeal *newAppeal = (Appeal *) malloc(sizeof(Appeal));
    if (newAppeal == NULL) {
        printf("内存分配失败\n");
        return;
    }
    newAppeal->appealId = nextAppealId;
    nextAppealId++;
    strcpy(newAppeal->studentName, currentUser);
    printf("请输入你的学号: ");
    scanf("%d", &newAppeal->studentId);
    clearInputBuffer();
    printf("请输入原成绩: ");
    scanf("%f", &newAppeal->originalScore);
    clearInputBuffer();
    printf("请输入申请的新成绩: ");
    scanf("%f", &newAppeal->requestedScore);
    clearInputBuffer();
    printf("请输入申诉理由(0-199位): ");
    fgets(newAppeal->reason, sizeof(newAppeal->reason), stdin);
    int length = strlen(newAppeal->reason);
    if (newAppeal->reason[length - 1] != '\n') {
        printf("输入过长 \n");
        clearInputBuffer();
        return;
    }
    newAppeal->reason[strcspn(newAppeal->reason, "\n")] = '\0';
    strcpy(newAppeal->status, "待处理");
    newAppeal->next = NULL;
    if (appealHead == NULL) {
        // 添加到链表
        appealHead = newAppeal;
    } else {
        Appeal *last = appealHead;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = newAppeal;
    }
    appealCount++;
    printf("申诉已提交,申诉ID：%d\n", newAppeal->appealId);
    saveAppealsToFile();
}

// 查看所有申诉
void showAllAppeals() {
    if (appealHead == NULL) {
        printf("暂无申诉记录\n");
        return;
    }
    printf("申诉记录列表 \n");
    printf("%-6s %-10s %-6s %-8s %-8s %-10s %s\n",
           "ID", "学生", "学号", "原成绩", "申请成绩", "状态", "理由");
    printf("--------------------------------------------------------\n");
    Appeal *current = appealHead;
    while (current != NULL) {
        printf("%-6d %-10s %-6d %-8.2f %-8.2f %-10s %s\n",
               current->appealId,
               current->studentName,
               current->studentId,
               current->originalScore,
               current->requestedScore,
               current->status,
               current->reason);
        current = current->next;
    }
    printf("--------------------------------------------------------\n");
    printf("共 %d 条申诉记录\n", appealCount);
}

// 更新申诉状态
void updateAppealStatus() {
    if (strcmp(currentRole, "admin") != 0) {
        printf("只有管理员可以更新申诉状态\n");
        return;
    }
    if (appealHead == NULL) {
        printf("暂无申诉记录\n");
        return;
    }
    int appealId;
    printf("请输入要更新状态的申诉ID: ");
    scanf("%d", &appealId);
    clearInputBuffer();
    Appeal *current = appealHead;
    while (current != NULL) {
        if (current->appealId == appealId) {
            printf("当前状态：%s\n", current->status);
            printf("请选择新状态：\n");
            printf("1. 待处理\n");
            printf("2. 已处理\n");
            printf("3. 已拒绝\n");
            printf("请选择: ");
            int Choice;
            scanf("%d", &Choice);
            clearInputBuffer();
            switch (Choice) {
                case 1: {
                    strcpy(current->status, "待处理");
                    printf("状态已更新为：待处理\n");
                    break;
                }
                case 2: {
                    strcpy(current->status, "已处理");
                    printf("状态已更新为：已处理\n");
                    break;
                }
                case 3: {
                    strcpy(current->status, "已拒绝");
                    printf("状态已更新为：已拒绝\n");
                    break;
                }
                default: {
                    printf("无效选择\n");
                    return;
                }
            }
            saveAppealsToFile();
            return;
        }
        current = current->next;
    }
    printf("未找到ID为 %d 的申诉\n", appealId);
}

// 用户注册（管理员才能注册新用户）
void registerUser() {
    if (strcmp(currentRole, "admin") != 0) {
        printf("只有管理员可以注册新用户\n");
        return;
    }
    User newUser;
    FILE *fp = fopen("users.txt", "a+");
    if (fp == NULL) {
        printf("无法打开用户文件\n");
        return;
    }
    printf(" 用户注册 \n");
    printf("请输入用户名(1-20位字母和数字组合): ");
    fgets(newUser.userName, sizeof(newUser.userName), stdin);
    int length = strlen(newUser.userName);
    if (newUser.userName[length - 1] != '\n') {
        printf("字数不在范围（1-20） \n");
        clearInputBuffer();
        fclose(fp);
        return;
    }
    position = strcspn(newUser.userName, "\n");
    newUser.userName[position] = '\0';
    if (position == 0 || position > 20) {
        printf("字数不在范围（1-20） \n");
        fclose(fp);
        return;
    }
    position--;
    while (position >= 0) {
        if (!isalnum(newUser.userName[position])) {
            printf("用户名不合规\n");
            fclose(fp);
            return;
        }
        position--;
    }
    printf("请选择注册角色:\n");
    printf("1. 学生\n");
    printf("2. 老师\n");
    printf("3. 管理员\n");
    int roleChoice;
    scanf("%d", &roleChoice);
    clearInputBuffer();
    if (roleChoice == 1) strcpy(newUser.role, "student");
    else if (roleChoice == 2) strcpy(newUser.role, "teacher");
    else if (roleChoice == 3) strcpy(newUser.role, "admin");
    else {
        printf("无效选择\n");
        fclose(fp);
        return;
    }
    User temp;
    rewind(fp);
    while (fscanf(fp, "%s %s %s", temp.userName, temp.password, temp.role) != EOF) {
        if (strcmp(temp.userName, newUser.userName) == 0) {
            printf("用户名已存在\n");
            fclose(fp);
            return;
        }
    }
    printf("请输入密码(1-20位字母和数字组合): ");
    fgets(newUser.password, sizeof(newUser.password), stdin);
    length = strlen(newUser.password);
    if (newUser.password[length - 1] != '\n') {
        printf("字数不在范围（1-20） \n");
        clearInputBuffer();
        fclose(fp);
        return;
    }
    position = strcspn(newUser.password, "\n");
    newUser.password[position] = '\0';
    if (position == 0 || position > 20) {
        printf("字数不在范围（1-20） \n");
        fclose(fp);
        return;
    }
    position--;
    while (position >= 0) {
        if (!isalnum(newUser.password[position])) {
            printf("密码不合规\n");
            fclose(fp);
            return;
        }
        position--;
    }
    fprintf(fp, "%s %s %s\n", newUser.userName, newUser.password, newUser.role);
    fclose(fp);
    printf("注册成功\n");
    return;
}

// 用户登录
void login() {
    char username[50], password[50];
    FILE *fp = fopen("users.txt", "r");
    printf("用户登录 \n");
    if (fp == NULL) {
        printf("暂无用户，请先注册\n");
        return;
    }
    printf("用户名: ");
    fgets(username, sizeof(username), stdin);
    position = strcspn(username, "\n");
    username[position] = '\0';
    printf("密码: ");
    fgets(password, sizeof(password), stdin);
    position = strcspn(password, "\n");
    password[position] = '\0';
    User temp;
    int found = 0;
    while (fscanf(fp, "%s %s %s", temp.userName, temp.password, temp.role) != EOF) {
        if (strcmp(temp.userName, username) == 0 && strcmp(temp.password, password) == 0) {
            found = 1;
            strcpy(currentUser, username);
            strcpy(currentRole, temp.role);
            break;
        }
    }
    fclose(fp);
    if (found) {
        printf("登录成功\n");
    } else {
        printf("用户名或密码错误\n");
    }
    return;
}

// 修改自己密码（所有人可用）
void changeMyPassword() {
    char oldPassword[50], newPassword[50];
    FILE *fp = fopen("users.txt", "r");
    FILE *tempFp = fopen("temp.txt", "w");
    if (fp == NULL || tempFp == NULL) {
        printf("文件操作失败\n");
        return;
    }
    printf("修改密码\n");
    printf("请输入旧密码: ");
    fgets(oldPassword, sizeof(oldPassword), stdin);
    int length = strlen(oldPassword);
    if (oldPassword[length - 1] != '\n') {
        printf("字数不在范围（1-20） \n");
        clearInputBuffer();
        fclose(fp);
        fclose(tempFp);
        return;
    }
    position = strcspn(oldPassword, "\n");
    oldPassword[position] = '\0';
    if (position == 0 || position > 20) {
        printf("字数不在范围（1-20） \n");
        fclose(fp);
        fclose(tempFp);
        return;
    }
    User temp;
    int found = 0;
    while (fscanf(fp, "%s %s %s", temp.userName, temp.password, temp.role) != EOF) {
        if (strcmp(temp.userName, currentUser) == 0) {
            if (strcmp(temp.password, oldPassword) == 0) {
                found = 1;
                printf("请输入新密码(1-20位字母和数字组合): ");
                fgets(newPassword, sizeof(newPassword), stdin);
                length = strlen(newPassword);
                if (newPassword[length - 1] != '\n') {
                    printf("字数不在范围（1-20） \n");
                    clearInputBuffer();
                    fprintf(tempFp, "%s %s %s\n", temp.userName, temp.password, temp.role);
                    continue;
                }
                position = strcspn(newPassword, "\n");
                newPassword[position] = '\0';
                if (position == 0 || position > 20) {
                    printf("字数不在范围（1-20） \n");
                    fprintf(tempFp, "%s %s %s\n", temp.userName, temp.password, temp.role);
                    continue;
                }
                int flag = 1;
                for (int i = 0; i < position; i++) {
                    if (!isalnum(newPassword[i])) {
                        flag = 0;
                        break;
                    }
                }
                if (!flag) {
                    printf("密码不合规\n");
                    fprintf(tempFp, "%s %s %s\n", temp.userName, temp.password, temp.role);
                } else {
                    fprintf(tempFp, "%s %s %s\n", temp.userName, newPassword, temp.role);
                    printf("密码修改成功\n");
                }
            } else {
                printf("旧密码错误\n");
                fprintf(tempFp, "%s %s %s\n", temp.userName, temp.password, temp.role);
            }
        } else {
            //录入其余人信息
            fprintf(tempFp, "%s %s %s\n", temp.userName, temp.password, temp.role);
        }
    }
    fclose(fp);
    fclose(tempFp);
    remove("users.txt");
    rename("temp.txt", "users.txt");
    if (!found) {
        printf("用户不存在\n");
    }
    return;
}

// 管理员：修改任何人密码
void adminChangePassword() {
    if (strcmp(currentRole, "admin") != 0) {
        printf("只有管理员可以修改他人密码\n");
        return;
    }
    char targetUser[50], newPassword[50];
    FILE *fp = fopen("users.txt", "r");
    FILE *tempFp = fopen("temp.txt", "w");
    if (fp == NULL || tempFp == NULL) {
        printf("文件操作失败\n");
        return;
    }
    printf("修改用户密码\n");
    printf("请输入要修改密码的用户名: ");
    fgets(targetUser, sizeof(targetUser), stdin);
    int length = strlen(targetUser);
    if (targetUser[length - 1] != '\n') {
        printf("字数不在范围（1-20） \n");
        clearInputBuffer();
        fclose(fp);
        fclose(tempFp);
        return;
    }
    position = strcspn(targetUser, "\n");
    targetUser[position] = '\0';
    if (position == 0 || position > 20) {
        printf("字数不在范围（1-20） \n");
        fclose(fp);
        fclose(tempFp);
        return;
    }
    position--;
    while (position >= 0) {
        if (!isalnum(targetUser[position])) {
            printf("用户名不合规\n");
            fclose(fp);
            return;
        }
        position--;
    }
    User temp;
    int found = 0;
    while (fscanf(fp, "%s %s %s", temp.userName, temp.password, temp.role) != EOF) {
        if (strcmp(temp.userName, targetUser) == 0) {
            found = 1;
            printf("请输入新密码(1-20位字母和数字组合): ");
            fgets(newPassword, sizeof(newPassword), stdin);
            length = strlen(newPassword);
            if (newPassword[length - 1] != '\n') {
                printf("字数不在范围（1-20） \n");
                clearInputBuffer();
                fprintf(tempFp, "%s %s %s\n", temp.userName, temp.password, temp.role);
                continue;
            }
            position = strcspn(newPassword, "\n");
            newPassword[position] = '\0';
            if (position == 0 || position > 20) {
                printf("字数不在范围（1-20） \n");
                fprintf(tempFp, "%s %s %s\n", temp.userName, temp.password, temp.role);
                continue;
            }
            int flag = 1;
            for (int i = 0; i < position; i++) {
                if (!isalnum(newPassword[i])) {
                    flag = 0;
                    break;
                }
            }
            if (!flag) {
                printf("密码不合规\n");
                fprintf(tempFp, "%s %s %s\n", temp.userName, temp.password, temp.role);
            } else {
                fprintf(tempFp, "%s %s %s\n", temp.userName, newPassword, temp.role);
                printf("密码修改成功\n");
            }
        } else {
            //录入其余人信息
            fprintf(tempFp, "%s %s %s\n", temp.userName, temp.password, temp.role);
        }
    }
    fclose(fp);
    fclose(tempFp);
    remove("users.txt");
    rename("temp.txt", "users.txt");
    if (!found) {
        printf("用户不存在\n");
    }
    return;
}

// 管理员：批量导入账号（从文件）
void adminImportUsers() {
    if (strcmp(currentRole, "admin") != 0) {
        printf("只有管理员可以导入账号\n");
        return;
    }
    char filename[100];
    printf("请输入要导入的文件名(1-99位字符): ");
    fgets(filename, sizeof(filename), stdin);
    int length = strlen(filename);
    if (filename[length - 1] != '\n') {
        printf("文件名过长\n");
        clearInputBuffer();
        return;
    }
    position = strcspn(filename, "\n");
    filename[position] = '\0';
    if (position == 0) {
        printf("文件名不能为空\n");
        return;
    }
    FILE *importFp = fopen(filename, "r");
    if (importFp == NULL) {
        printf("无法打开文件\n");
        return;
    }
    FILE *userFp = fopen("users.txt", "a+");
    if (userFp == NULL) {
        printf("无法打开用户文件\n");
        fclose(importFp);
        return;
    }
    char userName[50];
    char password[50];
    char role[20];
    int count = 0;
    while (fscanf(importFp, "%s %s %s", userName, password, role) == 3) {
        fprintf(userFp, "%s %s %s\n", userName, password, role);
        count++;
    }
    fclose(importFp);
    fclose(userFp);
    printf("成功导入 %d 个用户\n", count);
    return;
}

// 管理员导出所有账号
void adminExportUsers() {
    if (strcmp(currentRole, "admin") != 0) {
        printf("只有管理员可以导出账号\n");
        return;
    }
    char filename[100];
    printf("请输入要导入的文件名(1-99位字符): ");
    fgets(filename, sizeof(filename), stdin);
    int length = strlen(filename);
    if (filename[length - 1] != '\n') {
        printf("文件名过长\n");
        clearInputBuffer();
        return;
    }
    position = strcspn(filename, "\n");
    filename[position] = '\0';
    if (position == 0) {
        printf("文件名不能为空\n");
        return;
    }
    FILE *userFp = fopen("users.txt", "r");
    FILE *exportFp = fopen(filename, "w");
    if (userFp == NULL || exportFp == NULL) {
        printf("文件操作失败\n");
        return;
    }
    char userName[50];
    char password[50];
    char role[20];
    int count = 0;
    while (fscanf(userFp, "%s %s %s", userName, password, role) == 3) {
        fprintf(exportFp, "%s %s %s\n", userName, password, role);
        count++;
    }
    fclose(userFp);
    fclose(exportFp);
    printf("成功导出 %d 个用户到 %s\n", count, filename);
    return;
}

// 保存数据到文件
void saveDataToFile() {
    FILE *fp = fopen("students.dat", "wb");
    if (fp == NULL) {
        printf("无法打开文件保存数据\n");
        return;
    }
    Student *current = head;
    int count = 0;
    while (current != NULL) {
        fwrite(current, sizeof(Student), 1, fp);
        current = current->next;
        count++;
    }
    fclose(fp);
    printf("数据已保存 %d 条记录到 students.dat\n", count);
    return;
}

// 添加学生（仅老师和管理员可用）
void addStudent() {
    if (strcmp(currentRole, "student") == 0) {
        printf("学生不能添加学生信息\n");
        return;
    }

    Student *newNode = (Student *) malloc(sizeof(Student));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return;
    }
    printf(" 添加学生\n");
    printf("请输入学号: ");
    scanf("%d", &newNode->id);
    clearInputBuffer();
    Student *current = head;
    while (current != NULL) {
        if (current->id == newNode->id) {
            printf("学号已存在，添加失败！\n");
            free(newNode);
            return;
        }
        current = current->next;
    }
    printf("请输入姓名(1-20位字母组合): ");
    fgets(newNode->name, sizeof(newNode->name), stdin);
    int length = strlen(newNode->name);
    if (newNode->name[length - 1] != '\n') {
        printf("字数不在范围（1-20） \n");
        clearInputBuffer();
        free(newNode);
        return;
    }
    position = strcspn(newNode->name, "\n");
    newNode->name[position] = '\0';
    if (position == 0 || position > 20) {
        printf("字数不在范围（1-20） \n");
        free(newNode);
        return;
    }
    for (int i = 0; i < position; i++) {
        if (!isalpha(newNode->name[i])) {
            printf("姓名只能包含字母\n");
            free(newNode);
            return;
        }
    }
    printf("请输入性别(M/W): ");
    scanf("%c", &newNode->gender);
    clearInputBuffer();
    printf("请输入年龄: ");
    scanf("%d", &newNode->age);
    clearInputBuffer();
    printf("请输入成绩: ");
    scanf("%f", &newNode->score);
    clearInputBuffer();
    newNode->next = NULL;
    if (head == NULL) {
        head = newNode;
    } else {
        Student *last = head;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = newNode;
    }
    studentCount++;
    printf("学生添加成功，当前共有 %d 名学生\n", studentCount);
    saveDataToFile();
    return;
}

// 显示所有学生（所有人可用）
void displayAllStudents() {
    if (head == NULL) {
        printf("暂无学生\n");
        return;
    }
    printf(" 学生列表 ：\n");
    printf("%-10s %-15s %-6s %-6s %-8s\n", "学号", "姓名", "性别", "年龄", "成绩");
    printf("------------------------------\n");
    Student *current = head;
    while (current != NULL) {
        printf("%-10d %-15s %-6c %-6d %-8.2f\n",
               current->id, current->name, current->gender,
               current->age, current->score);
        current = current->next;
    }
    printf("------------------------------\n");
    printf("共 %d 条记录\n", studentCount);
    return;
}

// 查找学生（所有人可用）
void searchStudent() {
    if (head == NULL) {
        printf("暂无学生\n");
        return;
    }
    printf("查找学生：\n");
    printf("请选择查找方式: \n");
    printf("1. 按学号查找\n");
    printf("2. 按姓名查找\n");
    int choice;
    scanf("%d", &choice);
    clearInputBuffer();
    int found = 0;
    Student *current = head;
    if (choice == 1) {
        int searchId;
        printf("请输入要查找的学号: ");
        scanf("%d", &searchId);
        clearInputBuffer();
        while (current != NULL) {
            if (current->id == searchId) {
                printf("找到学生信息：\n");
                printf("学号：%d\n", current->id);
                printf("姓名：%s\n", current->name);
                printf("性别：%c\n", current->gender);
                printf("年龄：%d\n", current->age);
                printf("成绩：%.2f\n", current->score);
                found = 1;
                break;
            }
            current = current->next;
        }
    } else if (choice == 2) {
        char searchName[50];
        printf("请输入要查找的姓名:  \n");
        fgets(searchName, sizeof(searchName), stdin);
        position = strcspn(searchName, "\n");
        searchName[position] = '\0';
        while (current != NULL) {
            if (strcmp(current->name, searchName) == 0) {
                printf("找到学生信息：\n");
                printf("学号：%d\n", current->id);
                printf("姓名：%s\n", current->name);
                printf("性别：%c\n", current->gender);
                printf("年龄：%d\n", current->age);
                printf("成绩：%.2f\n", current->score);
                found = 1;
            }
            current = current->next;
        }
    } else {
        printf("无效选择\n");
        return;
    }
    if (!found) {
        printf("未找到匹配的学生\n");
    }
    return;
}

// 修改学生信息（仅老师和管理员可用）
void modifyStudent() {
    if (strcmp(currentRole, "student") == 0) {
        printf("学生不能修改学生信息\n");
        return;
    }
    if (head == NULL) {
        printf("暂无学生\n");
        return;
    }
    printf("修改学生信息\n");
    printf("请输入要修改的学生学号: ");
    int modifyId;
    scanf("%d", &modifyId);
    clearInputBuffer();
    Student *current = head;
    while (current != NULL) {
        if (current->id == modifyId) {
            printf("\n当前学生信息：\n");
            printf("学号：%d\n", current->id);
            printf("姓名：%s\n", current->name);
            printf("性别：%c\n", current->gender);
            printf("年龄：%d\n", current->age);
            printf("成绩：%.2f\n", current->score);
            printf("\n请依次输入新的信息：\n");
            char input[100];
            printf("姓名: ");
            fgets(input, sizeof(input), stdin);
            position = strcspn(input, "\n");
            input[position] = '\0';
            strcpy(current->name, input);
            printf("性别: ");
            fgets(input, sizeof(input), stdin);
            current->gender = input[0];
            int newAge;
            printf("年龄: ");
            scanf("%d", &newAge);
            clearInputBuffer();
            current->age = newAge;
            double newScore;
            printf("成绩: ");
            scanf("%lf", &newScore);
            clearInputBuffer();
            current->score = newScore;
            printf("学生信息修改成功\n");
            saveDataToFile();
            return;
        }
        current = current->next;
    }
    printf("未找到该学号的学生\n");
    return;
}

// 删除学生（仅老师和管理员可用）
void deleteStudent() {
    if (strcmp(currentRole, "student") == 0) {
        printf("学生不能删除学生信息\n");
        return;
    }
    if (head == NULL) {
        printf("暂无学生\n");
        return;
    }
    printf("删除学生 \n");
    printf("请输入要删除的学生学号: ");
    int deleteId;
    scanf("%d", &deleteId);
    clearInputBuffer();
    Student *current = head;
    Student *prev = NULL;
    while (current != NULL) {
        if (current->id == deleteId) {
            printf("即将删除的学生信息：\n");
            printf("学号：%d\n", current->id);
            printf("姓名：%s\n", current->name);
            printf("性别：%c\n", current->gender);
            printf("年龄：%d\n", current->age);
            printf("成绩：%.2f\n", current->score);
            printf("确定删除吗？(y/n): ");
            char confirm;
            scanf("%c", &confirm);
            clearInputBuffer();
            if (confirm == 'y' || confirm == 'Y') {
                if (prev == NULL) {
                    head = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current);
                studentCount--;
                printf("学生删除成功\n");
                saveDataToFile();
            } else {
                printf("取消删除。\n");
            }
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("未找到该学号的学生\n");
    return;
}

// 在任意位置插入学生（仅老师和管理员可用）
void insertStudent() {
    if (strcmp(currentRole, "student") == 0) {
        printf("学生不能插入学生信息\n");
        return;
    }
    if (head == NULL) {
        printf("数据为空，将添加到开头\n");
        addStudent();
        return;
    }
    printf(" 插入学生 \n");
    printf("当前学生列表：\n");
    displayAllStudents();
    int insertPos;
    printf("请输入要插入到的位置（1-%d）: ", studentCount + 1);
    scanf("%d", &insertPos);
    clearInputBuffer();
    if (insertPos < 1 || insertPos > studentCount + 1) {
        printf("无效的位置\n");
        return;
    }
    Student *newNode = (Student *) malloc(sizeof(Student));
    if (newNode == NULL) {
        printf("内存分配失败\n");
        return;
    }
    printf("请输入学号: ");
    scanf("%d", &newNode->id);
    clearInputBuffer();
    Student *current = head;
    while (current != NULL) {
        if (current->id == newNode->id) {
            printf("学号已存在，插入失败\n");
            free(newNode);
            return;
        }
        current = current->next;
    }
    printf("请输入姓名: ");
    fgets(newNode->name, sizeof(newNode->name), stdin);
    position = strcspn(newNode->name, "\n");
    newNode->name[position] = '\0';
    printf("请输入性别(M/W): ");
    scanf("%c", &newNode->gender);
    clearInputBuffer();
    printf("请输入年龄: ");
    scanf("%d", &newNode->age);
    clearInputBuffer();
    printf("请输入成绩: ");
    scanf("%f", &newNode->score);
    clearInputBuffer();
    if (insertPos == 1) {
        newNode->next = head;
        head = newNode;
    } else {
        Student *prev = head;
        for (int i = 1; i < insertPos - 1; i++) {
            prev = prev->next;
        }
        newNode->next = prev->next;
        prev->next = newNode;
    }
    studentCount++;
    printf("学生插入位置成功\n");
    saveDataToFile();
    return;
}

//从文件加载数据
void loadDataFromFile() {
    FILE *fp = fopen("students.dat", "rb");
    if (fp == NULL) {
        printf("没有找到数据文件，将创建新文件\n");
        return;
    }
    Student *current = head; // 清空当前链表
    while (current != NULL) {
        Student *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    studentCount = 0;
    Student temp; // 从文件读取数据
    Student *last = NULL;
    while (fread(&temp, sizeof(Student), 1, fp) == 1) {
        Student *newNode = (Student *) malloc(sizeof(Student));
        if (newNode == NULL) {
            printf("内存分配失败\n");
            break;
        }
        *newNode = temp;
        newNode->next = NULL;
        if (head == NULL) {
            head = newNode;
            last = head;
        } else {
            last->next = newNode;
            last = last->next;
        }
        studentCount++;
    }
    fclose(fp);
    printf("数据共加载 %d 条记录\n", studentCount);
    return;
}

// 老师导出学生信息到文件
void teacherExportStudents() {
    if (strcmp(currentRole, "student") == 0) {
        printf("学生不能导出数据\n");
        return;
    }
    char filename[100];
    printf("请输入要导入的文件名(1-99位字符): ");
    fgets(filename, sizeof(filename), stdin);
    int length = strlen(filename);
    if (filename[length - 1] != '\n') {
        printf("文件名过长\n");
        clearInputBuffer();
        return;
    }
    position = strcspn(filename, "\n");
    filename[position] = '\0';
    if (position == 0) {
        printf("文件名不能为空\n");
        return;
    }
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法创建文件\n");
        return;
    }
    Student *current = head;
    int count = 0;
    while (current != NULL) {
        fprintf(fp, "%d,%s,%c,%d,%.2f\n",
                current->id, current->name, current->gender,
                current->age, current->score);
        current = current->next;
        count++;
    }
    fclose(fp);
    printf("成功导出 %d 条学生记录到 %s\n", count, filename);
    return;
}

// 排序功能（老师和管理员可用）
void sortStudents() {
    if (strcmp(currentRole, "student") == 0) {
        printf("学生不能排序\n");
        return;
    }
    if (head == NULL || head->next == NULL) {
        printf("数据太少，无需排序\n");
        return;
    }
    printf("排序 \n");
    printf("1. 按学号排序\n");
    printf("2. 按成绩排序\n");
    printf("3. 按年龄排序\n");
    printf("请选择排序方式: ");
    int sortChoice;
    scanf("%d", &sortChoice);
    clearInputBuffer();
    printf("1. 升序\n");
    printf("2. 降序\n");
    printf("请选择排序顺序: ");
    int orderChoice;
    scanf("%d", &orderChoice);
    clearInputBuffer();
    int ascending = orderChoice % 2;

    // 冒泡排序
    int flag;
    Student *temp1 = NULL;
    Student *temp2 = NULL;
    do {
        flag = 0;
        temp1 = head;
        while (temp1->next != temp2) {
            int needSwap = 0;
            if (sortChoice == 1) {
                if (ascending) {
                    needSwap = (temp1->id > temp1->next->id);
                } else {
                    needSwap = (temp1->id < temp1->next->id);
                }
            } else if (sortChoice == 2) {
                if (ascending) {
                    needSwap = (temp1->score > temp1->next->score);
                } else {
                    needSwap = (temp1->score < temp1->next->score);
                }
            } else if (sortChoice == 3) {
                if (ascending) {
                    needSwap = (temp1->age > temp1->next->age);
                } else {
                    needSwap = (temp1->age < temp1->next->age);
                }
            }
            if (needSwap) {
                int tempId = temp1->id;
                char tempName[50];
                strcpy(tempName, temp1->name);
                char tempGender = temp1->gender;
                int tempAge = temp1->age;
                float tempScore = temp1->score;

                temp1->id = temp1->next->id;
                strcpy(temp1->name, temp1->next->name);
                temp1->gender = temp1->next->gender;
                temp1->age = temp1->next->age;
                temp1->score = temp1->next->score;

                temp1->next->id = tempId;
                strcpy(temp1->next->name, tempName);
                temp1->next->gender = tempGender;
                temp1->next->age = tempAge;
                temp1->next->score = tempScore;
                flag = 1;
            }
            temp1 = temp1->next;
        }
        temp2 = temp1;
    } while (flag);
    printf("排序完成\n");
    displayAllStudents();
    saveDataToFile();
    return;
}

// 统计（所有人可用）
void statistics() {
    if (head == NULL) {
        printf("暂无学生数据\n");
        return;
    }
    printf("统计分析\n");
    Student *current = head;
    float totalScore = 0;
    float maxScore = 0;
    float minScore = 1000;
    int manCount = 0, womanCount = 0;
    int under18 = 0, between18_22 = 0, above22 = 0;
    int totalAge = 0;
    while (current != NULL) {
        totalScore += current->score;
        totalAge += current->age;
        if (current->score > maxScore) maxScore = current->score;
        if (current->score < minScore) minScore = current->score;
        if (current->gender == 'M' || current->gender == 'm') manCount++;
        else womanCount++;
        if (current->age < 18) under18++;
        else if (current->age <= 22) between18_22++;
        else above22++;
        current = current->next;
    }
    printf("总学生数：%d\n", studentCount);
    printf("平均成绩：%.2f\n", totalScore / studentCount);
    printf("平均年龄：%.2f\n", (float) totalAge / studentCount);
    printf("最高成绩：%.2f\n", maxScore);
    printf("最低成绩：%.2f\n", minScore);
    printf("性别统计：\n");
    printf("男生：%d人 (%.2f%%)\n", manCount, (float) manCount / studentCount * 100);
    printf("女生：%d人 (%.2f%%)\n", womanCount, (float) womanCount / studentCount * 100);
    printf("年龄分布：\n");
    printf("18岁以下：%d人\n", under18);
    printf("18-22岁：%d人\n", between18_22);
    printf("22岁以上：%d人\n", above22);
    return;
}

// 学生菜单
void showStudentMenu() {
    printf("==========================\n");
    printf("   学生管理系统 [学生]\n");
    printf("   当前用户：%s\n", currentUser);
    printf("==========================\n");
    printf("2. 查看所有学生\n");
    printf("3. 查找学生\n");
    printf("10. 统计信息\n");
    printf("11. 修改我的密码\n");
    printf("12. 成绩申诉\n");
    printf("13. 查看申诉记录\n");
    printf("0. 退出系统\n");
    printf("==========================\n");
    printf("请选择操作: ");
    return;
}

// 老师菜单
void showTeacherMenu() {
    printf("==========================\n");
    printf("   学生管理系统 [老师]\n");
    printf("   当前用户：%s\n", currentUser);
    printf("==========================\n");
    printf("1. 添加学生\n");
    printf("2. 查看所有学生\n");
    printf("3. 查找学生\n");
    printf("4. 修改学生信息\n");
    printf("5. 删除学生\n");
    printf("6. 插入学生\n");
    printf("7. 保存数据到文件\n");
    printf("8. 从文件加载数据\n");
    printf("9. 排序学生\n");
    printf("10. 统计信息\n");
    printf("11. 导出学生信息\n");
    printf("12. 修改我的密码\n");
    printf("13. 查看申诉记录\n");
    printf("0. 退出系统\n");
    printf("==========================\n");
    printf("请选择操作: ");
    return;
}

// 管理员菜单
void showAdminMenu() {
    printf("==========================\n");
    printf("   学生管理系统 [管理员]\n");
    printf("   当前用户：%s\n", currentUser);
    printf("==========================\n");
    printf("1. 添加学生\n");
    printf("2. 查看所有学生\n");
    printf("3. 查找学生\n");
    printf("4. 修改学生信息\n");
    printf("5. 删除学生\n");
    printf("6. 插入学生\n");
    printf("7. 保存数据到文件\n");
    printf("8. 从文件加载数据\n");
    printf("9. 排序学生\n");
    printf("10. 统计信息\n");
    printf("11. 导出学生信息\n");
    printf("12. 注册新用户\n");
    printf("13. 批量导入账号\n");
    printf("14. 导出所有账号\n");
    printf("15. 修改他人密码\n");
    printf("16. 修改我的密码\n");
    printf("17. 查看申诉记录\n");
    printf("18. 更新申诉状态\n");
    printf("0. 退出系统\n");
    printf("==========================\n");
    printf("请选择操作: ");
    return;
}

int main() {
    printf("欢迎使用学生管理系统\n");
    FILE *fp = fopen("users.txt", "r");
    if (fp == NULL) {
        fp = fopen("users.txt", "w");
        if (fp != NULL) {
            fprintf(fp, "admin admin123 admin\n");
            fclose(fp); // 创建完成后关闭
            printf("已创建默认管理员账号 admin/admin123\n");
        }
    } else {
        fclose(fp);
    }
    while (1) {
        printf("1. 登录\n");
        printf("2. 退出\n");
        printf("请选择: ");
        int loginChoice;
        scanf("%d", &loginChoice);
        clearInputBuffer();
        if (loginChoice == 1) {
            login();
            if (strlen(currentUser) > 0) break;
        } else if (loginChoice == 2) {
            printf("感谢使用，再见\n");
            return 0;
        } else {
            printf("无效选择\n");
        }
    }
    loadDataFromFile(); //加载学生数据
    loadAppealsFromFile(); // 加载申诉数据
    int choice;
    while (1) {
        if (strcmp(currentRole, "student") == 0) {
            showStudentMenu();
        } else if (strcmp(currentRole, "teacher") == 0) {
            showTeacherMenu();
        } else {
            showAdminMenu();
        }
        scanf("%d", &choice);
        clearInputBuffer();
        if (choice == 2) {
            // 公共功能（所有人可用）
            displayAllStudents();
        } else if (choice == 3) {
            searchStudent();
        } else if (choice == 10) {
            statistics();
        } else if (choice == 0) {
            saveAppealsToFile();
            printf("感谢使用，再见\n");
            Student *current = head; //释放内存
            while (current != NULL) {
                Student *temp = current;
                current = current->next;
                free(temp);
            }
            Appeal *aCurrent = appealHead;
            while (aCurrent != NULL) {
                Appeal *aTemp = aCurrent;
                aCurrent = aCurrent->next;
                free(aTemp);
            }
            return 0;
        } else if (strcmp(currentRole, "student") == 0) {
            // 学生专用
            if (choice == 11) {
                changeMyPassword();
            } else if (choice == 12) {
                studentAppeal();
            } else if (choice == 13) {
                showAllAppeals();
            } else {
                printf("无效选择\n");
            }
        } else if (strcmp(currentRole, "teacher") == 0) {
            // 老师专用
            switch (choice) {
                case 1: {
                    addStudent();
                    break;
                }
                case 4: {
                    modifyStudent();
                    break;
                }
                case 5: {
                    deleteStudent();
                    break;
                }
                case 6: {
                    insertStudent();
                    break;
                }
                case 7: {
                    saveDataToFile();
                    break;
                }
                case 8: {
                    loadDataFromFile();
                    break;
                }
                case 9: {
                    sortStudents();
                    break;
                }
                case 11: {
                    teacherExportStudents();
                    break;
                }
                case 12: {
                    changeMyPassword();
                    break;
                }
                case 13: {
                    showAllAppeals();
                    break;
                }
                default: {
                    if (choice != 2 && choice != 3 && choice != 10) {
                        printf("无效选择\n");
                    }
                }
            }
        }
        // 管理员专用
        else if (strcmp(currentRole, "admin") == 0) {
            switch (choice) {
                case 1: {
                    addStudent();
                    break;
                }
                case 4: {
                    modifyStudent();
                    break;
                }
                case 5: {
                    deleteStudent();
                    break;
                }
                case 6: {
                    insertStudent();
                    break;
                }
                case 7: {
                    saveDataToFile();
                    break;
                }
                case 8: {
                    loadDataFromFile();
                    break;
                }
                case 9: {
                    sortStudents();
                    break;
                }
                case 11: {
                    teacherExportStudents();
                    break;
                }
                case 12: {
                    registerUser();
                    break;
                }
                case 13: {
                    adminImportUsers();
                    break;
                }
                case 14: {
                    adminExportUsers();
                    break;
                }
                case 15: {
                    adminChangePassword();
                    break;
                }
                case 16: {
                    changeMyPassword();
                    break;
                }
                case 17: {
                    showAllAppeals();
                    break;
                }
                case 18: {
                    updateAppealStatus();
                    break;
                }
                default: {
                    if (choice != 2 && choice != 3 && choice != 10) {
                        printf("无效选择\n");
                    }
                }
            }
        }
        printf("按回车键继续... \n");
        getchar();
    }
    return 0;
}
