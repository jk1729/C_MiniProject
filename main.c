#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <emscripten/emscripten.h>

#define MAX 100

// ---------- STRUCT ----------
struct Student {
    char regNo[10];
    char name[50];
    int total;
    int present;
};

struct Student s[MAX];
int count = 0;

// ---------- STATE ----------
int mode = 0, step = 0;
// ---------- FUNCTION DECLARATIONS ----------
void fullReport();
void shortageReport();
void classesNeeded();
void classAverage();
char tempReg[10], markReg[10], delReg[10], searchReg[10];
char tempName[50];

// ---------- VALIDATION ----------
int isValidRegNo(char str[]) {
    if(strlen(str) != 9) return 0;

    if(!(str[0]>='0' && str[0]<='9' && str[1]>='0' && str[1]<='9'))
        return 0;

    for(int i=2;i<=4;i++)
        if(!(str[i]>='a' && str[i]<='z'))
            return 0;

    for(int i=5;i<=8;i++)
        if(!(str[i]>='0' && str[i]<='9'))
            return 0;

    return 1;
}

// ---------- FILE ----------
void save() {
    FILE *fp = fopen("/data/attendance.txt", "w");
    if (!fp) return;

    for (int i = 0; i < count; i++)
        fprintf(fp, "%s %s %d %d\n",
                s[i].regNo, s[i].name,
                s[i].total, s[i].present);

    fclose(fp);
}

void load() {
    FILE *fp = fopen("/data/attendance.txt", "r");
    if (!fp) return;

    count = 0;
    while (count < MAX &&
        fscanf(fp, "%s %s %d %d",
        s[count].regNo,
        s[count].name,
        &s[count].total,
        &s[count].present) != EOF) {
        count++;
    }
    fclose(fp);
}

// ---------- MENU ----------
EMSCRIPTEN_KEEPALIVE
void showMenu() {
    printf("\n------ MENU ------\n");
    printf("1. Add Student\n");
    printf("2. Mark Attendance\n");
    printf("3. Full Report\n");
    printf("4. Shortage Report\n");
    printf("5. Classes Needed\n");
    printf("6. Class Average\n");
    printf("7. Delete Student\n");
    printf("8. Search Student\n");
    printf("Enter your choice:\n");

    mode = 10;
}

// ---------- MENU HANDLER ----------
void handleMenu(char input[]) {
    int choice = atoi(input);

    switch (choice) {
        case 1: printf("Enter Reg No:\n"); mode=1; step=1; break;
        case 2: printf("Enter Reg No:\n"); mode=2; step=1; break;
        case 3: printf("\n"); fullReport(); showMenu(); break;
        case 4: printf("\n"); shortageReport(); showMenu(); break;
        case 5: printf("\n"); classesNeeded(); showMenu(); break;
        case 6: printf("\n"); classAverage(); showMenu(); break;
        case 7: printf("Enter Reg No to delete:\n"); mode=3; break;
        case 8: printf("Enter Reg No to search:\n"); mode=4; break;
        default: printf("Invalid choice!\n");
    }
}

// ---------- BUTTON FUNCTIONS ----------
EMSCRIPTEN_KEEPALIVE
void startAddStudent(){ mode=1; step=1; printf("Enter Reg No:\n"); }

EMSCRIPTEN_KEEPALIVE
void startMarkAttendance(){ mode=2; step=1; printf("Enter Reg No:\n"); }

EMSCRIPTEN_KEEPALIVE
void deleteStudent(){ mode=3; printf("Enter Reg No to delete:\n"); }

EMSCRIPTEN_KEEPALIVE
void searchStudent(){ mode=4; printf("Enter Reg No to search:\n"); }

// ---------- INPUT ----------
EMSCRIPTEN_KEEPALIVE
void handleInput(char input[]) {

    load();

    // MENU
    if (mode == 10) { handleMenu(input); return; }

    // ---------- ADD ----------
    if (mode == 1) {

        if (step == 1) {

            // 🔴 VALIDATION
            if (!isValidRegNo(input)) {
                printf("❌ Invalid RegNo! Format: 12abc1234\n");
                printf("Enter Reg No again:\n");
                return;
            }

            // 🔴 DUPLICATE CHECK
            for(int i=0;i<count;i++){
                if(strcmp(s[i].regNo, input) == 0){
                    printf("⚠️ RegNo already exists!\n");
                    printf("Enter Reg No again:\n");
                    return;
                }
            }

            // ✅ VALID
            strcpy(tempReg, input);
            printf("Enter Name:\n");
            step = 2;
        }

        else {
            strncpy(tempName, input, 49);
            tempName[49] = '\0';

            strcpy(s[count].regNo, tempReg);
            strcpy(s[count].name, tempName);
            s[count].total = 0;
            s[count].present = 0;

            count++;
            save();

            printf("✅ Student Added Successfully!\n");
            showMenu();
        }
    }

    // ---------- MARK ----------
    else if (mode == 2) {

        if(step == 1){

            if (!isValidRegNo(input)) {
                printf("Invalid RegNo!\n");
                showMenu();
                return;
            }

            strcpy(markReg, input);
            printf("Enter Status (P/A):\n");
            step = 2;
        }

        else {
            for(int i=0;i<count;i++){
                if(strcmp(s[i].regNo, markReg)==0){
                    s[i].total++;

                    if(input[0]=='P'||input[0]=='p')
                        s[i].present++;

                    save();
                    printf("Attendance Marked!\n");
                    showMenu();
                    return;
                }
            }
            printf("Student Not Found!\n");
            showMenu();
        }
    }

    // ---------- DELETE ----------
    else if(mode == 3){

        if (!isValidRegNo(input)) {
            printf("Invalid RegNo!\n");
            showMenu();
            return;
        }

        strcpy(delReg, input);

        for(int i=0;i<count;i++){
            if(strcmp(s[i].regNo, delReg)==0){
                for(int j=i;j<count-1;j++)
                    s[j]=s[j+1];

                count--;
                save();

                printf("Student Deleted Successfully!\n");
                showMenu();
                return;
            }
        }
        printf("Student Not Found!\n");
        showMenu();
    }

    // ---------- SEARCH ----------
    else if(mode == 4){

        if (!isValidRegNo(input)) {
            printf("Invalid RegNo!\n");
            showMenu();
            return;
        }

        strcpy(searchReg, input);

        for(int i=0;i<count;i++){
            if(strcmp(s[i].regNo, searchReg)==0){

                float per = (s[i].total==0)?0:
                            (s[i].present*100.0)/s[i].total;

                printf("RegNo: %s | Name: %s | Classes: %d/%d | Attendance: %.2f%%\n",
                       s[i].regNo, s[i].name,
                       s[i].present, s[i].total, per);

                showMenu();
                return;
            }
        }
        printf("Student Not Found!\n");
        showMenu();
    }
}

// ---------- REPORT ----------
EMSCRIPTEN_KEEPALIVE
void fullReport(){
    load();
    printf("\n--- FULL REPORT ---\n");

    for(int i=0;i<count;i++){
        float per=(s[i].total==0)?0:
                  (s[i].present*100.0)/s[i].total;

        printf("RegNo: %s | Name: %s | Classes: %d/%d | Attendance: %.2f%%\n",
               s[i].regNo,s[i].name,
               s[i].present,s[i].total,per);
    }
}

// ---------- SHORTAGE ----------
EMSCRIPTEN_KEEPALIVE
void shortageReport(){
    load();
    printf("\n--- SHORTAGE STUDENTS ---\n");

    for(int i=0;i<count;i++){
        float per=(s[i].total==0)?0:
                  (s[i].present*100.0)/s[i].total;

        if(per<75)
            printf("RegNo: %s | Name: %s | Attendance: %.2f%%\n",
                   s[i].regNo,s[i].name,per);
    }
}

// ---------- CLASSES NEEDED ----------
EMSCRIPTEN_KEEPALIVE
void classesNeeded(){
    load();

    for(int i=0;i<count;i++){
        int t=s[i].total, a=s[i].present;
        float per=(t==0)?0:(a*100.0)/t;

        if(per<75){
            int x = ((75*t) - (100*a) + 24)/25;
            if(x<0) x=0;

            printf("%s (%s) needs to attend %d classes to maintain above 75%%\n",
                   s[i].name, s[i].regNo, x);
        }
    }
}

// ---------- CLASS AVERAGE ----------
EMSCRIPTEN_KEEPALIVE
void classAverage(){
    load();

    float sum=0;
    for(int i=0;i<count;i++){
        float per=(s[i].total==0)?0:
                  (s[i].present*100.0)/s[i].total;
        sum+=per;
    }

    float avg=(count==0)?0:sum/count;
    printf("Class Average = %.2f%%\n",avg);
}

// ---------- MAIN ----------
int main() {
    system("mkdir /data");   // 🔥 critical fix
    load();
    showMenu();
    return 0;
}