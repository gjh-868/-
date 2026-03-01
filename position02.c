#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    int id; // 学号
    char name[50]; // 姓名
    char gender; // 性别 M/W
    int age; // 年龄
    float score; // 成绩
    struct Student *next;
} Student;

int position = 0;
Student *head = NULL;
int studentCount = 0; // 学生总数

// 清空输入缓冲区
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    };
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
            printf("内存分配失败！\n");
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

// 主菜单
void showMenu() {
    printf("\n ==========================\n");
    printf("   学生管理系统 \n");
    printf("==========================\n");
    printf("1. 添加学生\n");
    printf("2. 显示所有学生\n");
    printf("3. 查找学生\n");
    printf("4. 修改学生信息\n");
    printf("5. 删除学生\n");
    printf("6. 插入学生\n");
    printf("7. 保存数据到文件\n");
    printf("8. 从文件加载数据\n");
    printf("0. 退出系统\n");
    printf("==========================\n");
    printf("请选择操作: ");
}

int main() {
    printf("欢迎使用学生管理系统\n");
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
