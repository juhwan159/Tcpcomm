#include <stdio.h> // 표준 입출력
#include <arpa/inet.h> // 인터넷 프로토콜
#include <unistd.h> // 유닉스 표준
#include <stdlib.h> // 표준 라이브러리
#include <string.h> // 문자열 처리함수 
#include <sys/socket.h> // 소켓통신 함수

#define BUF_SIZE 1024 // 1024 바이트 버퍼 크기 사용
void error_handling (char *message); // 에러 처리 함수(계속 재사용)
int main(int argc, char *argv[]) // 쉘에서 입력되는 인자 갯수와 배열
{				// (argument count, argument value)
	int serv_sock, clnt_sock; // 서버 소켓과 클라이언트 소켓 2개 필요
	// 서버소켓은 받아들이는 일만하고, 실제 통신은 클라이언트 소켓이 한다.
	char message[BUF_SIZE];  // 메시지 송신용 버퍼 1024 바이트
	int str_len, i; // 수신한 데이터의 길이와 제어변수 i(index), 관례

	struct sockaddr_in serv_adr; // 서버용 소켓(accept 용)
	struct sockaddr_in clnt_adr; // 클라이언트 소켓(서버소켓이 accept해주면 생성)
	socklen_t clnt_adr_sz; // 클라이언트 주소 구조체의 바이트 사이즈(크기)

	if(argc!=2) //shell에서 포트번호를 지정하지 않았으면
	{
		printf("Usage : %s <port>\n", argv[0]); // 사용자에게 사용법 안내
		exit(1); // 비정상 종료 처리
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0); // TCP 통신용 서버 소켓 생성
	if(serv_sock==-1) // 만들기 실패 하면
		error_handling("socket() error"); // 에러 표시
	
	memset(&serv_adr, 0, sizeof(serv_adr)); // 서버 주소 구조체를 0으로 채운다.
	serv_adr.sin_family = AF_INET; // 서버 주소 체계를 인터넷 계열로 설정
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); // 서버는 주소 필요 없음(제자리 동작)
	serv_adr.sin_port = htons(atoi(argv[1])); // 서버가 사용할 포트번호
	
	// 소켓과 주소를 엮어준다.
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	// 소켓을 서버용으로 사용할 수 있게한다.
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	clnt_adr_sz = sizeof(clnt_adr); // 클라이언트 주소 구조체의 크기

	for(i=0;i<5;i++) // 5번만 클라이언트의 접속을 받아 들인다.
	{
		// 서버 프로그램은 여기서 BLOCK 된다. 손님이 올 때까지 듣고 있는다.
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		if(clnt_sock==-1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n", i+1);

		// accept가 성공하면 클라이언트 소켓 clnt_sock가 만들어 진다.
		// while문이 언제 끝날까?
		// 클라이언트가 소켓을 닫아버리면 0이 돌아온다.	
		while((str_len = read(clnt_sock, message, BUF_SIZE))!=0)
			write(clnt_sock, message, str_len);

		close(clnt_sock); // 클라이언트와 접속이 끊겼으면 클라이언트 소켓을 닫는다.
	}
	close(serv_sock);
	return 0;
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}



 

	

