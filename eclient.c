#include <stdio.h> // 표준 입출력
#include <stdlib.h> // 표준 라이브러리
#include <string.h> // 문자열 처리 함수
#include <unistd.h> // 유닉스 표준
#include <arpa/inet.h> // 인터넷 프로토콜
#include <sys/socket.h> // 소켓 함수들

#define BUF_SIZE 1024 // 소켓통신 문자열 버퍼의 크기는 1024바이트
void error_handling(char *message); // 오류처리 함수

int main(int argc, char *argv[]) // 쉘에서 ip와 port번호를 받는다.
{
	int sock; // 서버에 접속할 소켓
	char message[BUF_SIZE]; // 서버 보낼 메시지를 저장할 문자열 버퍼
	int str_len, recv_len, recv_cnt; // 송수신 메시지의 문자열 길이
	struct sockaddr_in serv_adr; // 접속할 서버의 주소(어딘지 미리 알아야 함)
	// ip와 port를 지정하지 않았으면 사용법을 알려준다.

	if(argc!=3)
	{
		printf("Usage : %s <IP> <PORT>\n", argv[0]); // 사용법 표시
		exit(1); // 비정상 종료
	}
	
	sock = socket(PF_INET, SOCK_STREAM, 0); // TCP(Stream) 프로토콜 생성
	if(sock==-1) // 안만들어 지면
		error_handling("socket() error"); // 에러 표시 후 종료

	memset(&serv_adr, 0, sizeof(serv_adr)); // 서버쪽 주소 구조체를 0으로 초기화
	serv_adr.sin_family = AF_INET; // 인터넷 주소체계 사용
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]); // 서버 ip를 구조체에 저장한다.
	serv_adr.sin_port=htons(atoi(argv[2])); // 서버 port를 구조체에 저장한다.

	// connect는 클라이언트 전용함수 이고, 서버는 accept 상태여야 한다.
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!"); // 연결 실패
	else
		puts("connected......"); // 연결 성공

	while(1) // 연결되고 나면 무한 루프 진입, 언제 끝나느냐?
	{
		fputs("Input message(Q to quit): ", stdout); // 메시지 입력 안내문
		fgets(message, BUF_SIZE, stdin); // 사용자로부터 메시지를 입력 받는다.

		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break; // 사용자가 Q나 q를 입력하면 무한루프 탈출

		str_len = write(sock, message, strlen(message));
		recv_len=0;
		while(recv_len<str_len)
		{
			recv_cnt=read(sock, &message[recv_len], BUF_SIZE-1);
			if(recv_cnt==-1)
				error_handling("read() error!");
			recv_len+=recv_cnt;
		}
	
		message[recv_len] = 0; // 보낼 때 NULL문자 안보내서, 받았을 때 넣어줘야한다.
		printf("Message from server : %s", message); // 메아리 된 메시지 표시
	}
	close(sock); // 종료하기 전에 시스템에서 얻어온 소켓을 돌려준다.
	return 0; // 프로그램 정상 종료
}

void error_handling(char *message) // 예외 처리함수
{
	fputs(message, stderr); // 예외를 표시하고
	fputc('\n', stderr); // 줄 한번 바꾸고
	exit(1); // 비정상 종료 처리
}

