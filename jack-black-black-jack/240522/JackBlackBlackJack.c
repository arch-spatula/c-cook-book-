#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DECK_UNIT 105
/*
 *  INFO: S는 스페이드, H는 하트, D는 다이아몬드, C는 클러버
 *  TODO: 나중에 모양들을 nerdfont로 표현하기
 */
#define CARD_SHAPE "SHDC"
#define CARD_SHAPE_SIZE 4
/*
 *  INFO: T는 화면서 10을 표현하지만 값은 T로 표현함 UI상 T를 10으로 처리하는
 *  것이 내부 셔플을 처리하는 것보다 쉬움
 */
#define CARD_VALUE "A23456789TJQK"
#define CARD_VALUE_SIZE 13

#define INIT_DECK                                                              \
  "SAS2S3S4S5S6S7S8S9STSJSQSKHAH2H3H4H5H6H7H8H9HTHJHQHKDAD2D3D4D5D6D7D8D9DTDJ" \
  "DQDKCAC2C3C4C5C6C7C8C9CTCJCQCK"

enum SESSION { HOUSE_BROKE, PLAYER_BROKE, END_GAME, PLAYING };

enum SESSION beforeRoundStart(int *rounds, int playerMoney, int dealerMoney);

enum STATUS { DEFAULT, STAY, BUST, BLACK_JACK };

typedef struct Person {
  int money;
  int wins;
  char cards[DECK_UNIT];
  enum STATUS status;
  int score;
} Person;

enum GAME_RESULT { PLAYER_WIN, DEALER_WIN, DRAW };

int placeBet(int playerMoney);

int exitSession(enum SESSION, int rounds, Person *player, Person *dealer);

void initDeck(char *deck);

void shuffle(char *deck);

void hit(char *deck, char *cards, int *drawIdx);

void decision(Person *player, Person *dealer, char *deck, int *drawIdx);

void playerDecision(Person *player, int *drawIdx, char *deck);

void dealerDecision(Person *player, int *drawIdx, char *deck);

void checkScore(Person *person);

void checkWin(Person *player, Person *dealer, enum GAME_RESULT *gameResult);

void transaction(Person *winner, Person *loser, int bet);

/*
 * 21에 딜러보다 더 가까이 만들면 이기는 게임
 * https://namu.wiki/w/%EB%B8%94%EB%9E%99%EC%9E%AD(%EC%B9%B4%EB%93%9C%EA%B2%8C%EC%9E%84)
 * 1 덱으로 플레이(프로토타입)
 * - Blackjack
 *   - A랑 JQK로 21 만들면 1.5배 배당
 *
 * 컴퓨터가 딜러
 * - 딜러는 8, 8이 나오더라도 스플릿 할 권한 없이 16으로 게임을 진행
 * - 딜러 블랙잭을 우선 순위로 모든 플레이어 (승리/패배)
 * - 딜러가 소프트 17에서 히트
 *   - 16 이하면 무조건 히트, 17 이상이면 무조건 스테이
 * - 버스트가 아니면 A는 11로 간주
 * - 딜러의 숫자 합이 블랙잭이 아닌 일반 21일 경우 무승부로 인정되어 베팅반환
 * - 딜러는 패 공개
 *
 * 플레이어는 사용자
 * - 플레이어가 버스트 되면 딜러가 무조건 이긴다
 * - 서렌더 허용
 *   - 플레이어가 게임을 포기하고, 베팅액의 절반을 돌려 받는 규칙
 * - AA 스플릿 후 또 A가 나왔을 시 다시 스플릿 허용
 * - 더블다운
 *   - 합이 21이 넘지 않는 한 무제한으로 뽑을 수 있는 카드를 이후 단 하나만 더
 * 받는 조건으로 돈을 두 배
 *   - 모든 경우 허용
 * - A유리한쪽으로 선택
 * - 인슈어런스
 *   - 딜러의 오픈된 카드가 스페이드 A일 경우, 딜러가 블랙잭이 나올 가능성에
 * 대비해 보험
 *   - 딜러가 블랙잭일 경우 딜러는 보험금의 4배를 보험수당으로 지불
 *   - 건 금액의 절반이 보험금으로 지불
 * - Five card Charlie
 *   - 플레이어 패가 5장 이되고 딜러가 블랙잭이 아니면 플레이어 승리
 *   - 딜러 5장에 블랙잭이 아니면 플레이어 승리
 * - 플레이어가 777로 블랙 잭하면 배당 7배
 * - 원아이드잭
 *   - 스페이드J 와 하트J이 같은 무늬의 A 가 조합된 블랙잭시 배당 2배
 * - Stay 카드 뽑기 넘기기
 * - 스플릿
 *   - 처음 받은 2장의 카드가 같은 숫자일 경우, 패를 두 개로 나누어 게임을
 * 동시에 두 번
 *   - 이후 또 똑같은 숫자가 나오면 계속해서 스플릿해서 진행 가능
 *   - 본인 재산만큼 베팅 가능
 * - Even Money
 *   - 플레이어가 블랙잭이 나오고, 딜러의 보여진 패가 스페이드 A인 경우 선택
 * 발동
 *   - 플레이어는 1배 보상을 받고 게임 종료 혹은 3배 보상으로 게임 지속
 *   - 게임 시작 시점에 인슈어런스 적용시 push 효과 적용 무승부로 게임 종료
 */
int main(void) {
  int rounds = 0;
  Person player = {1000, 0, {0}, DEFAULT, 0};
  Person dealer = {1000000, 0, {0}, DEFAULT, 0};
  enum SESSION session = PLAYING;
  int playerBet = 0;
  /*int i = 0;*/

  // NOTE: 덱
  // (2 ~ 10, J, Q, K, A) * (S, H, D, C) -> 13 * 4 = 52개 조합
  // 카드 1장 당 갖고 있어야 할 공간은 2바이트 클로버 A는 CA로 표현
  //  52장의 카드를 바이트단위로 갖고 있기 위해서는 104바이트가 필요함
  char deck[DECK_UNIT] = {0};
  int drawIdx = 0;

  // INFO: 덱 셔플(shuffle)은 rand 함수를 사용하고 있어서 시드값 선언이 필요함
  srand(time(NULL));

  // NOTE: 게임 시작 출력
  printf("Jack Black Black Jack\n\n");
  // TODO: 중요!!!!! 잭 블랙 아스키 아트 추가
  printf(
      "게임을 시작합니다. 도박사 닝겐 재산은 %d원입니다. 호구카지노 콤퓨타의 "
      "재산은 %d원입니다.\n\n",
      player.money, dealer.money);

  enum GAME_RESULT gameResult = DRAW;
  // NOTE: 판(round) 루프
  while (session == PLAYING) {
    session = beforeRoundStart(&rounds, player.money, dealer.money);
    if (session != PLAYING) {
      break;
    }

    // NOTE: 게임 루프
    printf("%d번째 판입니다.\n", rounds);
    playerBet = 0;
    drawIdx = 0;
    player.status = DEFAULT;
    dealer.status = DEFAULT;
    strcpy(player.cards, "");
    strcpy(dealer.cards, "");
    player.score = 0;
    dealer.score = 0;
    gameResult = DRAW;

    playerBet = placeBet(player.money);
    printf("베팅은 %d원입니다.\n", playerBet);

    initDeck(deck);
    shuffle(deck);

    // NOTE: 판 시작에 2장 뽑기
    hit(deck, dealer.cards, &drawIdx);
    hit(deck, dealer.cards, &drawIdx);

    hit(deck, player.cards, &drawIdx);
    hit(deck, player.cards, &drawIdx);
    // TODO: 다이, 인슈런스 그냥 진행 결정

    // NOTE: hit/stay decision 루프
    decision(&player, &dealer, deck, &drawIdx);
    checkWin(&player, &dealer, &gameResult);

    // NOTE: 배당
    switch (gameResult) {
    case PLAYER_WIN:
      transaction(&player, &dealer, playerBet);
      break;
    case DEALER_WIN:
      transaction(&dealer, &player, playerBet);
      break;
    case DRAW:
      break;
    }
    printf("[%d]판 닝겐: %d원 [%d]승, 콤퓨타 %d원 [%d]승\n", rounds,
           player.money, player.wins, dealer.money, dealer.wins);
  }

  // NOTE: 종료 방식에 따라 처리
  return exitSession(session, rounds, &player, &dealer);
}

// TODO: 플레이어가 블랙잭으로 승리하면 2배 배당 딜러 승리에는 없음
void transaction(Person *winner, Person *loser, int bet) {
  loser->money -= bet;
  winner->money += bet;
}

void checkWin(Person *player, Person *dealer, enum GAME_RESULT *gameResult) {
  // INFO: 동시에 BUST 혹은 BLACK_JACK일 경우 딜러 승리
  if ((player->status == BLACK_JACK && dealer->status == BLACK_JACK) ||
      (player->status == BUST && dealer->status == BUST)) {
    dealer->wins += 1;
    printf("딜러가 우선권으로 승리\n");
    *gameResult = DEALER_WIN;
  } else if (player->status == BLACK_JACK || dealer->status == BUST) {
    printf("플레이어 승리\n");
    player->wins += 1;
    *gameResult = PLAYER_WIN;
  } else if (player->status == BUST || dealer->status == BLACK_JACK) {
    printf("딜러 승리\n");
    dealer->wins += 1;
    *gameResult = DEALER_WIN;
  } else if (player->score < dealer->score) {
    printf("딜러 승리\n");
    dealer->wins += 1;
    *gameResult = DEALER_WIN;
  } else if (player->score > dealer->score) {
    printf("플레이어 승리\n");
    player->wins += 1;
    *gameResult = PLAYER_WIN;
  } else {
    printf("기적의 무승부\n");
    *gameResult = DRAW;
  }
  printf("본인: [%s]패 [%d]점 딜러: [%s]패[%d]점\n", player->cards,
         player->score, dealer->cards, dealer->score);
}

/*
 * 패를 추가하는 메서드
 */
void hit(char *deck, char *cards, int *drawIdx) {
  unsigned long cardIdx = strlen(cards);
  cards[cardIdx] = deck[*drawIdx];
  cards[cardIdx + 1] = deck[*drawIdx + 1];
  cards[cardIdx + 2] = '\0';
  *drawIdx += 2;
}

void playerDecision(Person *player, int *drawIdx, char *deck) {
  char choice = '\0';
  int waitChoice = 1;
  while (waitChoice) {
    waitChoice = 0;
    printf("입력: ");
    choice = getchar();
    // NOTE: 표준 입력 이후 즉시 버퍼 비우기
    while (getchar() != '\n') {
      continue;
    }
    if (choice == 'h' || choice == 'H') {
      hit(deck, player->cards, drawIdx);
    } else if (choice == 's' || choice == 'S') {
      player->status = STAY;
    } else {
      printf("\n잘못된 입력입니다.\n");
      waitChoice = 1;
    }
  }
}

void dealerDecision(Person *dealer, int *drawIdx, char *deck) {
  if (dealer->score <= 16) {
    hit(deck, dealer->cards, drawIdx);
  } else {
    dealer->status = STAY;
  }
}

void decision(Person *player, Person *dealer, char *deck, int *drawIdx) {
  while (player->status == DEFAULT || dealer->status == DEFAULT) {
    printf("딜러의 패: [%s]\n", dealer->cards);
    printf("본인의 패: [%s]\n", player->cards);
    printf("\n[h] 뽑기(Hit) \n[s] 중단(Stay)\n\n");
    if (player->status == DEFAULT) {
      playerDecision(player, drawIdx, deck);
    }
    if (dealer->status == DEFAULT) {
      dealerDecision(dealer, drawIdx, deck);
    }
    checkScore(player);
    checkScore(dealer);
    // NOTE: 게임의 결과가 결정되었기 때문에 바로 루프를 깨기
    if (player->status == BLACK_JACK || player->status == BUST ||
        dealer->status == BLACK_JACK || dealer->status == BUST) {
      break;
    }
  }
}

void checkScore(Person *person) {
  person->score = 0;
  int i = 0, countA = 0;
  char cardValue = '0';
  while (cardValue != '\0') {
    cardValue = person->cards[i];
    i += 1;
    if (i % 2 == 0) {
      if (cardValue == 'A') {
        countA += 1;
        continue;
      }
      if (cardValue == 'T' || cardValue == 'J' || cardValue == 'Q' ||
          cardValue == 'K') {
        person->score += 10;
        continue;
      }
      person->score += atoi(&cardValue);
    }
  }

  for (i = 0; i < countA; i++) {
    if (person->score >= 11) {
      person->score += 1;
    } else {
      person->score += 11;
    }
  }

  if (person->score == 21) {
    person->status = BLACK_JACK;
  }
  if (person->score > 21) {
    person->status = BUST;
  }
}

/*
 *  NOTE: 덱초기화
 * 덱의 주소는 필요합니다.
 */
void initDeck(char *deck) {
  int i = 0;
  for (i = 0; i < DECK_UNIT; i++) {
    deck[i] = INIT_DECK[i];
  }
  deck[DECK_UNIT] = '\0';
}

void shuffle(char *deck) {
  int i = 0, swapPoint = 0;
  char tempCard[3] = {"ii\0"};
  printf("\n");
  for (i = 102; i >= 0; i -= 2) {
    swapPoint = (rand() % 52) * 2;

    tempCard[0] = deck[i];
    tempCard[1] = deck[i + 1];

    deck[i] = deck[swapPoint];
    deck[i + 1] = deck[swapPoint + 1];

    deck[swapPoint] = tempCard[0];
    deck[swapPoint + 1] = tempCard[1];
  }
}

int placeBet(int playerMoney) {
  int playerBet = 0, stdIOCheck = 0;
  while (playerBet < playerMoney / 10 || playerBet > playerMoney) {
    printf(
        "베팅액을 정해주세요. 최소 %d부터 최대 %d 원까지 가능합니다.\n\n금액:",
        playerMoney / 10, playerMoney);
    stdIOCheck = scanf("%d%*c", &playerBet);
    if (stdIOCheck == 0) {
      printf("뇌절 오셨습니까? 문자는 숫자가 아닙니다.\n");
      while (getchar() != '\n') {
        continue;
      }
    }
    if (playerBet < playerMoney / 10) {
      printf("금액이 너무 작습니다.\n");
    }
    if (playerBet > playerMoney) {
      printf("뇌절 오셨습니까?\n");
    }
    if (playerBet == playerMoney / 10) {
      printf("최소 베팅 쌉하남자 오지는군요.\n");
    }
    if (playerBet == playerMoney) {
      printf("전재산 베팅 상남자 인정합니다.\n");
    }
  }
  return playerBet;
}

enum SESSION beforeRoundStart(int *rounds, int playerMoney, int dealerMoney) {
  if (playerMoney == 0) {
    return PLAYER_BROKE;
  }
  if (dealerMoney == 0) {
    return HOUSE_BROKE;
  }

  char choice = '\0';
  int waitChoice = 1;
  printf("\n[p] 게임하기(play) \n[e] 게임중단(end)\n\n\n");
  while (waitChoice) {

    waitChoice = 0;
    printf("입력: ");
    choice = getchar();
    // NOTE: 표준 입력 이후 즉시 버퍼 비우기
    while (getchar() != '\n') {
      continue;
    }
    if (choice == 'p' || choice == 'P') {
      *rounds += 1;
      return PLAYING;
    } else if (choice == 'e' || choice == 'E') {
      return END_GAME;
    } else {
      printf("잘못된 입력입니다.\n");
      waitChoice = 1;
    }
  }

  return END_GAME;
}

/*
 *  NOTE: 메뉴 선택에 따라 출력
 */
int exitSession(enum SESSION session, int rounds, Person *player,
                Person *dealer) {
  switch (session) {
  case HOUSE_BROKE:
    /*printf("Winner winner chicken dinner\n");*/
    printf("당신은 호구카지노를 털어먹었습니다. 닝겐 승리!!\n");
    return EXIT_SUCCESS;
  case PLAYER_BROKE:
    /*printf("Loser, loser reddit user\n");*/
    printf("당신은 호구카지노에게 당했습니다. 콤퓨타 승리!!\n");
    return EXIT_SUCCESS;
  case END_GAME:
    printf("[%d]판 닝겐: %d원 [%d]승, 콤퓨타 %d원 [%d]승\n", rounds,
           player->money, player->wins, dealer->money, dealer->wins);
    return EXIT_SUCCESS;
  case PLAYING:
    puts("ERROR: PLAYING이 될 수 없습니다.");
    return EXIT_FAILURE;
  default:
    puts("ERROR: 알 수 없는 에러입니다.");
    return EXIT_FAILURE;
  }
}
