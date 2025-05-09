
/*  call_stack

    실제 시스템에서는 스택이 메모리에 저장되지만, 본 과제에서는 `int` 배열을 이용하여 메모리를 구현합니다.
    원래는 SFP와 Return Address에 실제 가상 메모리 주소가 들어가겠지만, 이번 과제에서는 -1로 대체합니다.

    int call_stack[]      : 실제 데이터(`int 값`) 또는 `-1` (메타데이터 구분용)을 저장하는 int 배열
    char stack_info[][]   : call_stack[]과 같은 위치(index)에 대한 설명을 저장하는 문자열 배열

    ==========================call_stack 저장 규칙==========================
    매개 변수 / 지역 변수를 push할 경우   : int 값 그대로
    Saved Frame Pointer 를 push할 경우  : call_stack에서의 index
    반환 주소값을 push할 경우       : -1
    =======================================================================


    ==========================stack_info 저장 규칙==========================
    매개 변수 / 지역 변수를 push할 경우        : 변수에 대한 설명
    Saved Frame Pointer 를 push할 경우  : 어떤 함수의 SFP인지
    반환 주소값을 push할 경우                 : "Return Address"
    ========================================================================
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#define STACK_SIZE 50 // 최대 스택 크기


int     call_stack[STACK_SIZE];         // Call Stack을 저장하는 배열
char    stack_info[STACK_SIZE][20];     // Call Stack 요소에 대한 설명을 저장하는 배열

/*  SP (Stack Pointer), FP (Frame Pointer)

    SP는 현재 스택의 최상단 인덱스를 가리킵니다.
    스택이 비어있을 때 SP = -1, 하나가 쌓이면 `call_stack[0]` -> SP = 0, `call_stack[1]` -> SP = 1, ...

    FP는 현재 함수의 스택 프레임 포인터입니다.
    실행 중인 함수 스택 프레임의 sfp를 가리킵니다.
*/
int SP = -1;
int FP = -1;
int FP_stack[STACK_SIZE]; //각 함수의 FP를 저장하는 배열, 0으로 초기화

void func1(int arg1, int arg2, int arg3);
void func2(int arg1, int arg2);
void func3(int arg1);

/*
    현재 call_stack 전체를 출력합니다.
    해당 함수의 출력 결과들을 바탕으로 구현 완성도를 평가할 예정입니다.
*/
void print_stack()
{
    if (SP == -1)
    {
        printf("Stack is empty.\n");
        return;
    }

    printf("====== Current Call Stack ======\n");

    for (int i = SP; i >= 0; i--)
    {
        if (call_stack[i] != -1)
            printf("%d : %s = %d", i, stack_info[i], call_stack[i]);
        else if (FP_stack[i] != -1 && FP_stack[i]!=0)
            printf("%d : %s = %d", i, stack_info[i], FP_stack[i]);
        else
            printf("%d : %s", i, stack_info[i]);
        if (i == SP)
            printf("    <=== esp\n");
        else if (i == FP)
            printf("    <=== ebp\n");
        else
            printf("\n");
    }
    printf("================================\n\n");
}

//매개변수 배열, 매개변수 이름 배열, 매개변수 개수, 지역변수 배열, 지역변수 이름 배열, 지역변수 개수, 함수 이름을 인자로 받아 stack에 쌓는 함수
void push(int* args, char** arg_names, int arg_size, int* locals, char** local_names, int local_size, const char* func_name)
{
    for (int i = arg_size - 1; i >= 0; i--)
    {
        SP++;
        call_stack[SP] = args[i];
        strcpy(stack_info[SP], arg_names[i]);
    }

    SP++;
    call_stack[SP] = -1;
    strcpy(stack_info[SP], "Return address");

    SP++;
    call_stack[SP] = -1;
    sprintf(stack_info[SP], "%s SFP", func_name);
    FP_stack[SP] = FP;
    FP = SP;

    for (int j = 0; j < local_size; j++)
    {
        SP++;
        call_stack[SP] = locals[j];
        strcpy(stack_info[SP], local_names[j]);
    }


}

//매개변수 개수와 지역변수 개수를 받아 그것에 2를 더한 만큼을 stack에서 비우고 초기화하는 함수
void pop(int arg_size, int local_size)
{
    int total_pop = arg_size + local_size + 2;

    for (int k = 0; k < total_pop; k++)
    {
        call_stack[SP] = -1;
        strcpy(stack_info[SP], "");
        SP--;

        if (FP_stack[SP] != 0)
        {
            FP = FP_stack[SP];
            FP_stack[SP] = 0;
        }
    }
}

//func 내부는 자유롭게 추가해도 괜찮으나, 아래의 구조를 바꾸지는 마세요
void func1(int arg1, int arg2, int arg3)
{
    int var_1 = 100;

    int args[] = { arg1, arg2, arg3 };
    char* arg_names[] = { "arg1", "arg2", "arg3" };

    int locals[] = { var_1 };
    char* local_names[] = { "var_1" };

    push(args, arg_names, sizeof(args) / sizeof(int), locals, local_names, sizeof(locals) / sizeof(int), "func1");

    print_stack();
    func2(11, 13);

    // func2의 스택 프레임 제거 (함수 에필로그 + pop)
    pop(2, 1);

    print_stack();
}


void func2(int arg1, int arg2)
{
    int var_2 = 200;

    int args[] = { arg1, arg2 };
    char* arg_names[] = { "arg1", "arg2" };

    int locals[] = { var_2 };
    char* local_names[] = { "var_2" };

    push(args, arg_names, sizeof(args) / sizeof(int), locals, local_names, sizeof(locals) / sizeof(int), "func2");

    print_stack();
    func3(77);

    // func3의 스택 프레임 제거 (함수 에필로그 + pop)
    pop(1, 2);

    print_stack();
}


void func3(int arg1)
{
    int var_3 = 300;
    int var_4 = 400;

    int args[] = { arg1 };
    char* arg_names[] = { "arg1" };

    int locals[] = { var_3, var_4 };
    char* local_names[] = { "var_3", "var_4" };

    push(args, arg_names, sizeof(args) / sizeof(int), locals, local_names, sizeof(locals) / sizeof(int), "func3");

    // func3의 스택 프레임 형성 (함수 프롤로그 + push)

    print_stack();
}


//main 함수에 관련된 stack frame은 구현하지 않아도 됩니다.
int main()
{
    func1(1, 2, 3);

    // func1의 스택 프레임 제거 (함수 에필로그 + pop)
    pop(3,1);
    print_stack();
    return 0;
}