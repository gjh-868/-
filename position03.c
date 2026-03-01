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
} User;

int position = 0;
Student *head = NULL;
int studentCount = 0; // 学生总数
char currentUser[50] = ""; // 当前登录用户名称

// 清空输入缓冲区
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    };
}

// 用户注册
void registerUser() {
    User newUser;
    FILE *fp = fopen("users.txt", "a+");
    if (fp == NULL) {
        printf("无法打开用户文件\n");
        return;
    }
    printf(" 用户注册 \n");
    printf("请输入用户名(字母和数字): ");
    fgets(newUser.userName, sizeof(newUser.userName), stdin);
    position = strcspn(newUser.userName, "\n");
    newUser.userName[position] = '\0';
    if (position == 0) {
        printf("用户名不能为空\n");
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
    User temp;
    rewind(fp);
    while (fscanf(fp, "%s %s", temp.userName, temp.password) != EOF) {
        if (strcmp(temp.userName, newUser.userName) == 0) {
            printf("用户名已存在\n");
            fclose(fp);
            return;
        }
    }
    printf("请输入密码(字母和数字): ");
    fgets(newUser.password, sizeof(newUser.password), stdin);
    position = strcspn(newUser.password, "\n");
    newUser.password[position] = '\0';
    if (position == 0) {
        printf("密码不能为空\n");
        fclose(fp);
        return;
    }
    position--;
    while (position >= 0) {
        if (!isalnum(newUser.password[position])) {
            printf("密码不合规\n");
            return;
        }
        position--;
    }
    fprintf(fp, "%s %s\n", newUser.userName, newUser.password);
    fclose(fp);
    printf("注册成功,请登录\n");
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
    while (fscanf(fp, "%s %s", temp.userName, temp.password) != EOF) {
        if (strcmp(temp.userName, username) == 0 && strcmp(temp.password, password) == 0) {
            found = 1;
            strcpy(currentUser, username);
            break;
        }
    }
    fclose(fp);
    if (found) {
        printf("登录成功！欢迎 %s\n", username);
    } else {
        printf("用户名或密码错误\n");
    }
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
}

// 添加学生
void addStudent() {
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
}

// 显示所有学生
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
}

// 查找学生
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
}

// 修改学生信息
void modifyStudent() {
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
}

// 删除学生
void deleteStudent() {
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
}

// 在任意位置插入学生
void insertStudent() {
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
        } else {
            Student *last = head;
            while (last->next != NULL) {
                last = last->next;
            }
            last->next = newNode;
        }
        studentCount++;
    }
    fclose(fp);
    printf("数据共加载 %d 条记录\n", studentCount);
}

// 排序功能
void sortStudents() {
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
    int flag; //标记交换次数
    Student *temp1 = NULL;
    Student *temp2 = NULL;
    do {
        flag = 0;
        temp1 = head;
        while (temp1->next != temp2) {
            int needSwap = 0;
            if (sortChoice == 1) {
                // 按学号
                if (ascending) {
                    needSwap = (temp1->id > temp1->next->id);
                } else {
                    needSwap = (temp1->id < temp1->next->id);
                }
            } else if (sortChoice == 2) {
                // 按成绩
                if (ascending) {
                    needSwap = (temp1->score > temp1->next->score);
                } else {
                    needSwap = (temp1->score < temp1->next->score);
                }
            } else if (sortChoice == 3) {
                // 按年龄
                if (ascending) {
                    needSwap = (temp1->age > temp1->next->age);
                } else {
                    needSwap = (temp1->age < temp1->next->age);
                }
            }
            if (needSwap) {
                // 交换节点数据
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
}

// 统计
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
}

// 主菜单
void showMenu() {
    printf("==========================\n");
    printf("   学生管理系统\n");
    printf("   当前用户：%s\n", currentUser);
    printf("==========================\n");
    printf("1. 添加学生\n");
    printf("2. 显示所有学生\n");
    printf("3. 查找学生\n");
    printf("4. 修改学生信息\n");
    printf("5. 删除学生\n");
    printf("6. 插入学生\n");
    printf("7. 保存数据到文件\n");
    printf("8. 从文件加载数据\n");
    printf("9. 排序学生\n");
    printf("10. 统计信息\n");
    printf("0. 退出系统\n");
    printf("==========================\n");
    printf("请选择操作: ");
}

int main() {
    printf("欢迎使用学生管理系统\n");
    while (1) {
        printf("\n1. 登录\n");
        printf("2. 注册\n");
        printf("3. 退出\n");
        printf("请选择: ");
        int loginChoice;
        scanf("%d", &loginChoice);
        clearInputBuffer();
        if (loginChoice == 1) {
            login();
            if (strlen(currentUser) > 0) break;
        } else if (loginChoice == 2) {
            registerUser();
        } else if (loginChoice == 3) {
            printf("感谢使用，再见\n");
            return 0;
        } else {
            printf("无效选择\n");
        }
    }
    loadDataFromFile();
    int choice;
    while (1) {
        showMenu();
        scanf("%d", &choice);
        clearInputBuffer();
        switch (choice) {
            case 1: {
                addStudent();
                break;
            }
            case 2: {
                displayAllStudents();
                break;
            }
            case 3: {
                searchStudent();
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
            case 10: {
                statistics();
                break;
            }
            case 0: {
                printf("感谢使用，再见\n");
                Student *current = head;
                while (current != NULL) {
                    Student *temp = current;
                    current = current->next;
                    free(temp);
                }
                return 0;
            }
            default: {
                printf("无效选择，请重新输入\n");
            }
        }
        printf("\n按回车键继续...");
        getchar();
    }
    return 0;
}
