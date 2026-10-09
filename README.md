# SYSPROGweek06

## 1. 카운터를 volatile sig_atomic_t로 바꾸고, 핸들러에서는 카운터만 1 증가, printf()는 main()에서만 실행, 세 번 누르면 종료

기존에는 `static volatile sig_atomic_t got_sigint = 0;` 였지만,

이제는 몇 번 눌렀는지 세야 하므로 `static volatile sig_atomic_t sigint_count = 0;`


핸들러에서는
```
static void handler(int sig)
{
  (void)sig;
  sigint_count++;
}
```
처럼 카운터만 증가 그리고 main()에서 
```
while (sigint_count < 3)
    pause();
```
로 세 번째 SIGINT가 올 때까지 대기
io는 모두 main()에서 실행

## 실행 결과
<img width="680" height="227" alt="image" src="https://github.com/user-attachments/assets/22be7658-c8e7-49a5-b4f2-0d10bca4b36a" />


## 1. 알람을 반복해야 하므로 간격, 반복 횟수가 필요, 실행 인자로 간격과 반복 횟수를 받고, repeat_count를 감소시켜 반복 종료

기존의 `static volatile sig_atomic_t timeout = 0;` 을
```
static volatile sig_atomic_t interval_sec = 0;
static volatile sig_atomic_t repeat_count = 0;
```
간격, 반복 횟수로 변형

기존에는 alarm(3)으로 3초 뒤에 한 번만 SIGALRM을 받았지만, 이제는 실행할 때 간격과 반복 횟수를 입력받아야 하므로 main()을 다음과 같이 변경
`int main(int argc, char *argv[])`
./alarm 2 5로 실행하면 argv[1]에는 "2", argv[2]에는 "5"가 전달됨.

sscanf()로 입력값을 정수로 변환하고, 간격과 반복 횟수가 양수인지 확인
```
if (sscanf(argv[1], "%d", &interval) != 1 ||
    sscanf(argv[2], "%d", &repeats) != 1 ||
    interval <= 0 || repeats <= 0) {
    printf("간격과 반복횟수는 양의 정수여야 합니다.\n");
    return 1;
}
```

interval_sec는 알람 간격을 저장하고, repeat_count는 남은 알람 횟수를 저장
```
interval_sec = interval;
repeat_count = repeats;
```

## 2. repeat_count를 volatile sig_atomic_t로 선언하고, 핸들러에서는 남은 횟수를 1 감소
```
static void on_alarm(int sig)
{
    (void)sig;
    repeat_count--;

    if (repeat_count > 0)
        alarm(interval_sec);
}
```
반복할 횟수가 남아 있는지 검사, 횟수가 남아 있다면 alarm(interval_sec)을 다시 호출

## 3. 최초 알람을 예약하고, 반복이 끝날 때까지 main()에서 대기
```
alarm(interval_sec);

while (repeat_count > 0)
    sleep(1);
```
alarm(interval_sec)으로 첫 번째 알람을 예약
이후 repeat_count가 0보다 큰 동안 sleep(1)로 대기, 알람이 발생하면 핸들러가 repeat_count를 감소시키고, 마지막 알람 이후에는 반복문 종료

## 실행 결과


## 사용한 프롬프트

### 1. 메인 룰 템플릿
메인 룰:
1. 반드시 스킵 없이 하나하나 구체적으로 설명할 것(설명을 할 때 너무 난해한 경우에는 쉬운말로 풀어 써야됨, 지루하지 않도록 중간에 농담이나 재미있는 이야기도 섞을 것)
2. 잘못된 정보가 존재하는지 확인 하고, 존재한다면 공신력 있는 곳에서 다시 검색할 것(시간은 3분으로 제한하며, 그래도 발견이 안될 경우 정확한 정보를 찾지 못함을 알리며 다음 의견을 묻고, 수락 시 가장 비슷한 정보라도 알릴 것)
3. 시청각 자료 또는 실제로 사용되는 자료를 같이 사용하여 이해력을 돕도록 할 것(단, 토큰비용을 생각해서 가장 시청각 자료가 필요한 경우에만)
4. 사람이 이해하기 어려운 출력은 절대 하지 말 것

### 2. 카테고리 제시
카테고리: 코딩을 이용한 문제 해결

### 3. 구체적인 문제와 조건 제시
문제: 과제 내용은 기존의 1_sigint.c 프로그램을 변형해서 Ctrl+C(SIGINT)를 한 번 누르면 바로 종료되는 것이 아니라, 누른 횟수를 세고 세 번째 Ctrl+C를 눌렀을 때 종료하도록 만들어야 됨.

조건: 핸들러 안에서는 printf()를 사용하면 안됨. 그리고, 코드를 원본에서 너무 벗어나도록 작성하지 않을 것.

문제: 2_alarm.c 를 변형해서 ./alarm <간격초> <반복횟수> 형식으로 작성되도록 할 것.

조건: alarm 은 한 번만 울리므로, 핸들러가 처리한 뒤 다시 alarm(간격) 을 걸어야 됨. 코드를 원본에서 너무 벗어나도록 작성하지 않을 것.
