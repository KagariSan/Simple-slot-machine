#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define KNRM "\x1B[0m" // color to normal
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"
#define RESET "\033[0m" // color to normal

#ifndef MAX_NAME_LENGTH
#define MAX_NAME_LENGTH 5
#endif

#ifndef MAX_NAME_LENGTH_INPUT
#define MAX_NAME_LENGTH_INPUT "%5s"
#endif

#ifndef MAX_MONEY
#define MAX_MONEY
#endif

#ifndef MAX_SAVED_FILE
#define MAX_SAVED_FILE 5
#endif

#ifndef MSG_WELCOME
#define MSG_WELCOME KGRN "Welcome to slot machine game!" KNRM
#endif

#ifndef MSG_ERROR
#define MSG_ERROR KRED "Sad, Something go wrong!?" KNRM
#endif

#ifndef MSG_DATA_ERROR
#define MSG_DATA_ERROR                                                         \
  KRED "We can't load data file or saved file which very important!! Do you "  \
       "want reset config and delete all saved game?" KNRM
#endif

#ifndef MSG_MAIN_MENU
#define MSG_MAIN_MENU                                                          \
  "1. New game\n2. Load game\n3. Highscore\n4. Credit\n5. Quit"
#endif

#ifndef MSG_MENU_LOAD
#define MSG_MENU_LOAD KYEL "Welcome Back! Please choose your saved game!\n" KNRM
#endif

#ifndef MSG_PLAY_MENU
#define MSG_PLAY_MENU "1.Play slot machine\n2.Save \n3.Cashout \n4.Main menu"
#endif

#ifndef MSG_MONEY_RED
#define MSG_MONEY_RED KRED "Hey %s! You have $%0.2lf money right now" KNRM
#endif

#ifndef MSG_MONEY_GREEN
#define MSG_MONEY_GREEN KGRN "Hey %s! You have $%0.2lf money right now" KNRM
#endif

#ifndef MSG_LOAD_MENU_SAVED_LIST
#define MSG_LOAD_MENU_SAVED_LIST "%d name:\" %s\" money: %d"
#endif

#ifndef MAX_HIGHSCORE_TAPE
#define MAX_HIGHSCORE_TAPE 10
#endif

#ifndef MSG_ASK_USER_PASSWORD
#define MSG_ASK_USER_PASSWORD "Please enter your password (%d time left): "
#endif

#ifndef MSG_CASHOUT
#define MSG_CASHOUT "GOODBYE! Your score has been saved in Highscore"
#endif

// struct and static variable
typedef struct Player {
  char *name[MAX_NAME_LENGTH];
  double money;
  int password;
} Player;
struct Player *playing;
struct Player *savedPtr;
struct Player *highScoreTape;
char user_Input_string[MAX_NAME_LENGTH];
char user_Input_char = 'A';
FILE *ptr;
//
void user_Input_correct(char user_input[]);
int preLoad();
void password_handle();
void encryption();
void cheater_detector();
int main_Menu();
int new_Game();
int save_Game();
int load_Game();
void cash_Out();
void Highscore();
void credit();
void clear();
//
void clear() {
  while (getchar() != '\n') {
  }
}

void user_Input_correct(char user_input[]) {
  for (size_t i = 0; user_input[i] != '\0'; i++) {
    if (user_input[i] > 'A' && user_input[i] < 'Z')
      user_input[i] = (int)user_input[i] + 32;
    else if (user_input[i] > 'a' && user_input[i] < 'z')
      ;
    else
      for (size_t j = i; j < 30 - i - 1; j++) {
        user_input[j] = user_input[j + 1];
      }
  }
}

int preLoad() {
  // khởi tạo dữ liệu default
  for (size_t i = 0; i < MAX_SAVED_FILE; i++) {
    (savedPtr + sizeof(struct Player) * i)->money = 10;
    strcpy((char *)(savedPtr + sizeof(struct Player) * i)->name, "NONE");
  }
  for (size_t i = 0; i < MAX_HIGHSCORE_TAPE; i++) {
    (highScoreTape + sizeof(struct Player) * i)->money = 10;
    strcpy((char *)(highScoreTape + sizeof(struct Player) * i)->name, "NONE");
  }
  // mở file
  ptr = fopen("data.bin", "r+");
  // kiểm tra file có tồn tại hay không
  if (ptr == NULL) {
    puts(MSG_DATA_ERROR);
    fgets(user_Input_string, MAX_NAME_LENGTH, stdin);
    user_Input_correct(user_Input_string);
    // hỏi người dùng trước khi khởi tạo file save mới
    if (strcmp(user_Input_string, "YES")) {
      ptr = fopen("data.bin", "w+");
      // ghi các giá trị default vào file
      for (size_t i = 0; i < MAX_SAVED_FILE; i++) {
        fwrite(savedPtr, sizeof(struct Player), 1, ptr);
      }
      for (size_t i = 0; i < MAX_HIGHSCORE_TAPE; i++) {
        fwrite(highScoreTape, sizeof(struct Player), 1, ptr);
      }
      printf("done\n");
      fclose(ptr);
      return 0;
    } else if (strcmp(user_Input_string, "NO")) {
      puts(MSG_ERROR);
      exit(1);
    }
  }
  // kiểm tra tính toàn vẹn của dữ liệu
  // gọi hàm kiểm tra tính toàn vẹn ở đây, làm sau(đang để tạm 1 phần của hàm
  // kiểm tra ở đây)
  else {
    int size;
    // tiến hành đếm byte
    fseek(ptr, 0, 2);
    size = ftell(ptr);
    // ghi các giá trị default vào file nếu file không đầy đủ
    if (sizeof(struct Player) * (MAX_SAVED_FILE + MAX_HIGHSCORE_TAPE) != size) {
      printf("Damaged file, start to repair\n");
      fclose(ptr);
      ptr = fopen("data.bin", "w+");
      for (size_t i = 0; i < MAX_SAVED_FILE; i++) {
        fwrite(savedPtr, sizeof(struct Player), 1, ptr);
      }
      for (size_t i = 0; i < MAX_HIGHSCORE_TAPE; i++) {
        fwrite(highScoreTape, sizeof(struct Player), 1, ptr);
      }
      fclose(ptr);
      printf("Repair success!\n");
      return 0;
    }
    rewind(ptr);
    for (size_t i = 0; i < MAX_SAVED_FILE; i++) {
      //đọc file save
      fread(savedPtr + sizeof(struct Player) * i, sizeof(struct Player), 1,
            ptr);
    }
    for (size_t i = 0; i < MAX_SAVED_FILE; i++) {
      //đọc highScoreTape
      fread(highScoreTape + sizeof(struct Player) * i, sizeof(struct Player), 1,
            ptr);
    }
    for (size_t i = 0; i < MAX_SAVED_FILE; i++) {
      printf("Name : %s Money: %0.2f\n",
             (char *)(savedPtr + sizeof(struct Player) * i)->name,
             (savedPtr + sizeof(struct Player) * i)->money);
    }
  }
  fclose(ptr);
  return 0;
}

double machine() {
  // random và tạo hiệu ứng (hiệu ứng maaxx ngu, chỉnh sau)
  int score[3] = {0, 0, 0};
  char *color[] = {KMAG, KGRN, KBLU, KCYN, KMAG, KGRN, KBLU, KCYN, KMAG, KGRN};
  for (size_t i = 0; i < 10; i++) {
    score[0] = rand() % 10;
    score[1] = rand() % 10;
    score[2] = rand() % 10;
    // usleep(5000);
    printf("\r%s%d %s%d %s%d %s", color[score[0]], score[0], color[score[1]],
           score[1], color[score[2]], score[2], RESET);
  }
  for (size_t i = 0; i < 10; i++) {
    score[1] = rand() % 10;
    score[2] = rand() % 10;
    // usleep(5000);
    printf("\r%s%d %s%d %s%d %s", color[score[0]], score[0], color[score[1]],
           score[1], color[score[2]], score[2], RESET);
  }
  for (size_t i = 0; i < 10; i++) {
    score[2] = rand() % 10;
    // usleep(5000);
    printf("\r%s%d %s%d %s%d %s", color[score[0]], score[0], color[score[1]],
           score[1], color[score[2]], score[2], RESET);
  }
  if (score[0] == score[1] && score[1] == score[2]) {
    printf(KRED "\nYOLO, BIG WIN\n" KNRM);
    playing->money += 10;
    return 10;
  }
  if (score[0] == score[1] || score[1] == score[2] || score[2] == score[0]) {
    printf(KYEL "\nOK, NOT BAD\n" KNRM);
    playing->money += 0.5;
    return 0.5;
  }
  printf(KBLU "\nSAD\n" KNRM);
  playing->money -= 0.25;
  return -0.25;
}

void password_handle() {}

void encryption() {}

void cheater_detector() {}
// Main menu, sure vcl
int main_Menu() {
  printf("%s\n", MSG_MAIN_MENU);
  fscanf(stdin, " %c", &user_Input_char);
  clear();
  switch (user_Input_char) {
  case '1':
    // yêu cầu người dùng nhập vào và khởi những dữ liệu cơ bản (name, money) để
    // bắt đầu
    printf("Case 1\n");
    playing->money = 10;
    do {
      // fflush(stdin);
      printf("Enter your name(space or special char not allow, max %d "
             "character): ",
             MAX_NAME_LENGTH);
      scanf(MAX_NAME_LENGTH_INPUT, (char *)playing->name);
      fflush(stdin);
      // clear();
    } while (strcmp((char *)playing->name, "") == 0);
    srand(time(NULL)); // gieo seen
    while (new_Game() && playing->money > 0)
      ; // loop game function
    break;
  case '2':
    printf("%s\n", MSG_MENU_LOAD);
    if (load_Game()) {
      srand(time(NULL)); // gieo seen
      while (new_Game() && playing->money > 0)
        ; // loop game function
    }
    break;
  case '3':
    Highscore();
    break;
  case '4':
    credit();
    exit(255);
    break;
  case '5':
    exit(255);
  default:
    printf("Please enter right number\n");
    return 1;
  }
  return 1;
}

int new_Game() {
  user_Input_char = '0';
  // clrscr();
  if (playing->money > 5)
    printf(MSG_MONEY_GREEN, (char *)playing->name, playing->money);
  else
    printf(MSG_MONEY_RED, (char *)playing->name, playing->money);
  printf("\n%s\n", MSG_PLAY_MENU);
  // fflush(stdin);
  fflush(stdout);
  fscanf(stdin, " %c", &user_Input_char);
  clear();
  switch (user_Input_char) {
  case '1':
    machine();
    return 1;
  case '2':
    while (save_Game())
      ;
    return 0;
  case '3':
    cash_Out();
    return 0;
  case '4':
    return 0;
  default:
    printf("Correct input pls dude\n");
    return 0;
  }
}

int save_Game() {
  for (size_t i = 0; i < MAX_SAVED_FILE; i++) {
    printf("STT: %d || Name : %s || Money: %0.2f\n", (int)i + 1,
           (char *)(savedPtr + sizeof(struct Player) * i)->name,
           (savedPtr + sizeof(struct Player) * i)->money);
  }
  printf("0. Back!\n");
  int savepoint;
  fscanf(stdin, "%d", &savepoint); // scan savepoint player wanna save
  while (savepoint > MAX_SAVED_FILE || savepoint < 0) {
    printf("Invalid input");
    fscanf(stdin, "%d", &savepoint);
    if (savepoint == 0)
      return 0;
    return 1;
  }
  memcpy((savedPtr + sizeof(struct Player) * (savepoint - 1)), playing,
         sizeof(struct Player)); // copy save to list save at mem
  ptr = fopen("data.bin", "w+");
  rewind(ptr);
  // ghi các giá trị hiện tại vào file
  for (int i = 0; i < MAX_SAVED_FILE; i++) {
    fwrite((savedPtr + sizeof(struct Player) * i), sizeof(struct Player), 1,
           ptr);
  }
  for (int i = 0; i < MAX_HIGHSCORE_TAPE; i++) {
    fwrite((highScoreTape + sizeof(struct Player) * i), sizeof(struct Player),
           1, ptr);
  }
  printf("done\n");
  fclose(ptr);
  return 0;
}

int load_Game() {
  for (size_t i = 0; i < MAX_SAVED_FILE; i++) {
    printf("STT: %d || Name : %s || Money: %0.2f\n", (int)i + 1,
           (char *)(savedPtr + sizeof(struct Player) * i)->name,
           (savedPtr + sizeof(struct Player) * i)->money);
  }
  printf("0. Back!\n");
  int savepoint;
  fscanf(stdin, "%d", &savepoint); // scan savepoint player wanna save
  while (savepoint > MAX_SAVED_FILE || savepoint < 0) {
    printf("Invalid input");
    fscanf(stdin, "%d", &savepoint);
    if (savepoint == 0)
      return 0;
  }
  memcpy(playing, (savedPtr + sizeof(struct Player) * (savepoint - 1)),
         sizeof(struct Player)); // copy save from list save at mem
  printf("Loaded %s!", (char *)playing->name);
  return 1;
}

void cash_Out() {
  ptr = fopen("data.bin", "w+");
  rewind(ptr);
  // ghi các giá trị default vào file
  for (int i = 0; i < MAX_SAVED_FILE; i++) {
    fwrite((savedPtr + sizeof(struct Player) * i), sizeof(struct Player), 1,
           ptr);
  }
  for (int i = 0; i < MAX_HIGHSCORE_TAPE; i++) {
    if ((double)((highScoreTape + sizeof(struct Player) * i)->money) <
        (double)(playing->money)) {
      for (int j = i; j < MAX_HIGHSCORE_TAPE - 1; j++) {
        memcpy((highScoreTape + sizeof(struct Player) * j),
               (highScoreTape + sizeof(struct Player) * (j + 1)),
               sizeof(struct Player)); // move score down
      }
      memcpy(playing, (savedPtr + sizeof(struct Player) * i),
             sizeof(struct Player)); // copy score from list save at mem
      break;
    }
  }
  for (size_t i = 0; i < MAX_HIGHSCORE_TAPE; i++) {
    printf("STT: %d || Name : %s || Money: %0.2f\n", (int)i + 1,
           (char *)(highScoreTape + sizeof(struct Player) * i)->name,
           (highScoreTape + sizeof(struct Player) * i)->money);
  }
  for (int i = 0; i < MAX_HIGHSCORE_TAPE; i++) {
    fwrite((highScoreTape + sizeof(struct Player) * i), sizeof(struct Player),
           1, ptr);
  }
  fclose(ptr);
  printf(MSG_CASHOUT);
  exit(EXIT_SUCCESS);
}

void Highscore() {
  printf(KCYN "HighScore:\n" KNRM);
  for (size_t i = 0; i < MAX_HIGHSCORE_TAPE; i++) {
    printf("STT: %d || Name : %s || Money: %0.2f\n", (int)i + 1,
           (char *)(highScoreTape + sizeof(struct Player) * i)->name,
           (highScoreTape + sizeof(struct Player) * i)->money);
  }
}

void credit() {
  printf(KGRN"Created By"KRED"Vinh!!!"KNRM);
}

int main(int argc, char const *argv[]) {
  printf("%s\n", MSG_WELCOME);
  highScoreTape = malloc(sizeof(struct Player) * MAX_HIGHSCORE_TAPE);
  savedPtr = malloc(sizeof(struct Player) * MAX_SAVED_FILE);
  playing = malloc(sizeof(struct Player) * 1);
  while (preLoad());
  while (main_Menu());
  return 0;
}
