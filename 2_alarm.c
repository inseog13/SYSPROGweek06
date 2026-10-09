/*
 * 2_alarm.c — 정해진 시간 뒤에 시그널을 받는다
 *
 * [핵심 개념]
 *   alarm(n) 은 n 초 뒤에 커널이 SIGALRM 을 보내도록 예약한다.
 *   시간 제한(타임아웃)을 구현하는 가장 간단한 방법이다.
 *   출처: man 2 alarm, man 2 sigaction
 *
 * [컴파일·실행]
 *   gcc -Wall -Wextra -o 2_alarm 2_alarm.c
 *   ./2_alarm      # 3초 안에 뭔가 입력하지 않으면 시간이 끝난다
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t repeat_count = 0;
static volatile sig_atomic_t interval_sec = 0;

static void on_alarm(int sig)
{
    (void)sig;
    repeat_count--;

    if (repeat_count > 0) alarm(interval_sec);
}

int main(int argc, char *argv[])
{
    int interval, repeats;

    if (argc != 3) {
        printf("사용법: %s <간격초> <반복횟수>\n", argv[0]);
        return 1;
    }

    if (sscanf(argv[1], "%d", &interval) != 1 ||
        sscanf(argv[2], "%d", &repeats) != 1 ||
        interval <= 0 || repeats <= 0) {
        printf("간격과 반복횟수는 양의 정수여야 합니다.\n");
        return 1;
    }

    interval_sec = interval;
    repeat_count = repeats;

    struct sigaction sa;
    sa.sa_handler = on_alarm;   /* SIGALRM 이 오면 이 함수를 부르게 등록한다 */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;   /* SA_RESTART 를 안 줬으므로, 이 시그널은 fgets 같은 블로킹 호출을 중단시킨다 */
    sigaction(SIGALRM, &sa, NULL);

    printf("%d초 간격으로 %d회 알람을 실행합니다.\n",
           interval, repeats);
    fflush(stdout);   /* 프롬프트를 즉시 보이게 한다(버퍼에 남지 않도록) */
    alarm(interval_sec);   /* 3초 뒤 SIGALRM 예약 */

    sig_atomic_t previous_count = repeat_count;
    
    do {
    sleep(1);

    if (repeat_count != previous_count) {
            printf("SIGALRM을 받았습니다. 남은 횟수: %d\n",
                (int)repeat_count);
            fflush(stdout);
            previous_count = repeat_count;
        }
    } while (repeat_count > 0);

    alarm(0);   /* 입력을 받았으니 예약을 취소한다 */
    printf("타이머 종료\n");
    return 0;
}
