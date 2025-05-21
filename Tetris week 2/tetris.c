#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));
	createRankList(); // 실행되자마자 랭킹에 대한 정보 갱신
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}
	if (rankList != NULL)
	{ // rankList가 비어있지 않으면 하나씩 해제
		Node* del, *cur = rankList;
		while (cur->next != NULL)
		{
			del = cur; 
			cur = cur->next;
			del->next = NULL;
			free(del);
		}
		free(cur);
	}
	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2] = rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10); // 첫번째 nextBlock이 보여지는 부분
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	DrawBox(9, WIDTH + 10, 4, 8); // 두번째 nextBlock이 보여지는 부분
	
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		move(10 + i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID, int blockRotate, char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}
	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y, int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[currentBlock][blockRotate][i][j] == 1)
			{ // 블럭 중 칸을 차지하는 부분에 한해서 확인
				if (blockY + i > HEIGHT - 1)
					return 0;
				if (blockX + j > WIDTH - 1)
					return 0;
				if (blockX + j < 0)
					return 0; // 필드를 벗어났으면 오류이므로 return 0
				if (f[blockY + i][blockX + j] != 0)
					return 0; // 빈칸이 아니면 채울 수 없으므로 return 0
			}	
		}
	} // 다 통과했으면 채울 수 있으므로 return 1
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	int tempY = blockY, tempX = blockX, tempBlock = currentBlock, tempRotate = blockRotate;
	switch (command)
	{ // Command에 따라 이전 블록 지워주기
	case KEY_UP:
		tempRotate = (tempRotate + 3) % 4;
		break;
	case KEY_DOWN:
		tempY--;
		break;
	case KEY_RIGHT:
		tempX--;
		break;
	case KEY_LEFT:
		tempX++;
		break;
	default:
		break;
	} // 기존 블록 위치로 y좌표 옮기기
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) {
			if (block[tempBlock][tempRotate][i][j] == 1 && i + tempY >= 0) {
				move(i + tempY + 1, j + tempX + 1);
				printw(".");
			} // 기존 블록 지우기
		}
	}
	while (CheckToMove(field, tempBlock, tempRotate, tempY + 1, tempX))
		tempY++; // 기존 그림자 위치로 y좌표 옮기기
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) {
			if (block[tempBlock][tempRotate][i][j] == 1 && i + tempY >= 0) {
				move(i + tempY + 1, j + tempX + 1);
				printw(".");
			} // 기존 그림자 지우기
		}
	}
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	return;
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 
}

void BlockDown(int sig){
	timed_out = 0;
	int currentBlock = nextBlock[0];
	if (CheckToMove(field, currentBlock, blockRotate, blockY + 1, blockX))
		DrawChange(field, KEY_DOWN, currentBlock, blockRotate, ++blockY, blockX);
	else if (blockY != -1)
	{
		score += AddBlockToField(field, currentBlock, blockRotate, blockY, blockX);
		score += DeleteLine(field);
		PrintScore(score);
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % 7;
		blockY = -1, blockX = WIDTH / 2 - 2, blockRotate = 0;
		DrawField();
		DrawNextBlock(nextBlock);
	}
	else
	{
		gameOver = 1;
		DrawBlock(blockY, blockX, currentBlock, blockRotate, ' ');
		AddBlockToField(field, currentBlock, blockRotate, blockY, blockX);
	}
	return;
	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int touched = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[currentBlock][blockRotate][i][j] == 1)
			{ // 현재 그린 칸의 한 칸 밑에 이미 차있다면 다른 블록과 닿은 것, HEIGHT - 1이면 바닥과 닿은 것
				if (f[blockY + i + 1][blockX + j] == 1 || blockY + i == HEIGHT - 1)
					touched++; // 필드와 맞닿은 부분은 칸 당 수 증가
				f[blockY + i][blockX + j] = 1;
				
			}
		}
	}
	return touched * 10;
	//Block이 추가된 영역의 필드값을 바꾼다.
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i, j, cnt = 0;
	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
		{
			if (f[i][j] == 0)
				break;
			if (j == WIDTH - 1)
			{
				cnt++;
				for (int k = i; k > 1; k--)
					for (int l = 0; l < WIDTH; l++)
						f[k][l] = f[k - 1][l]; // 아랫줄을 깬 후 윗줄의 정보 한 칸씩 내리기
				for (int l = 0; l < WIDTH; l++)
					f[1][l] = 0; // 맨 윗줄은 다시 전부 0(빈 칸)으로 초기화
				DrawField(); // 다 찬 줄 깨고 필드 업데이트하기
				j = 0; // 밑 줄이 다시 채워졌을 수도 있으므로 다시 j = 0으로 초기화 후 업데이트
			}
		}
	}
	return cnt * cnt * 100;
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int newY = y;
	while (CheckToMove(field, blockID, blockRotate, newY + 1, x))
		newY++; // 모든 칸이 통과했으면 y좌표 한 칸 내리고 다시 반복
	DrawBlock(newY, x, blockID, blockRotate, '/'); // 맵 끝에 도달했으면 그냥 그리기
	return;
}
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
	return;
}

void createRankList() {
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE* fp;
	char userName[NAMELEN];
	int userNum, userScore;
	//1. 파일 열기
	fp = fopen("rank.txt", "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Failed to open file!");
		return;
	}
	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	if (fscanf(fp, "%d", &userNum) != EOF) { // rank.txt가 비어있지 않을 때 실행

		score_number = userNum; // score_number 전체 랭킹 개수 저장
		if (score_number > 0)
		{
			Node* curNode = (Node*)malloc(sizeof(Node)); // 첫 시작을 위한 dummy node
			Node* dummyHead = curNode;
			for (int i = 0; i < userNum; i++)
			{
				fscanf(fp, "%s%d", userName, &userScore);
				Node* newNode = (Node*)malloc(sizeof(Node));
				newNode->score = userScore;
				strcpy(newNode->name, userName);
				newNode->next = NULL; // 새로운 노드 생성 및 정보 저장
				curNode->next = newNode;
				curNode = newNode; // 뒤쪽 Node에 연결
			}
			rankList = dummyHead->next; // 랭킹 list의 head를 첫 Node로 연결
			dummyHead->next = NULL; // dummyHead 연결 제거
			curNode = NULL; // curNode 변수는 더 필요 없으므로 NULL로 초기화
			free(dummyHead); // dummyHead Node 제거
		}
	}
	else {
		printw("The ranking file is empty! Initialized file to 0\n");
		score_number = 0; // 비어있는 경우 score_number 자동으로 0으로 초기화
		fprintf(fp, "0\n"); // rank.txt가 아예 비어있을 경우 자동으로 0 추가
	}
	// 4. 파일닫기
	fclose(fp);
}

void rank() {
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X = 1, Y = score_number, ch; // X, Y는 default로 양 끝 값 저장
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. List ranks from X to Y\n");
	printw("2. List ranks by a specific name\n");
	printw("3. Delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);
	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		echo();
		char tempX[6] = { 0, }, tempY[6] = { 0, };
		printw("X: "); getnstr(tempX, 5);
		printw("Y: "); getnstr(tempY, 5); // 최대 5자리 숫자의 등수까지 저장 가능
		sscanf(tempX, "%d", &X);
		sscanf(tempY, "%d", &Y); // 만약 엔터가 들어오면 X, Y에는 default로 1, score_number가 저장
		X = (X > 1 ? X : 1); Y = (Y < score_number ? Y : score_number); // X와 Y의 범위에 따라 다시 초기화
		printw("\n      Rank       |    Score    \n");
		printw("--------------------------------\n");
		if (X <= Y && rankList != NULL)
		{
			Node* curNode = rankList; // Head로 시작 Node 저장
			int index = 1;
			while (curNode != NULL)
			{
				if (X <= index && index <= Y) // 범위 내에 있을 때부터 출력 시작
					printw("%-16s | %-8d\n", curNode->name, curNode->score);
				else if (index > Y)
					break;
				curNode = curNode->next;
				index++;
			}
		}
		else
			printw("Search failure: No rank in the list\n");
	}

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if (ch == '2') {
		char name[NAMELEN + 1];
		int check = 0;
		echo();
		printw("Input the name: ");
		getnstr(name, NAMELEN); // 이름 입력 받기
		printw("\n      Rank       |    Score    \n");
		printw("--------------------------------\n");
		if (rankList != NULL)
		{
			Node* curNode = rankList; // Head로 시작 Node 설정
			while (curNode != NULL)
			{
				if (strcmp(curNode->name, name) == 0) // 모든 Node 탐색하면서 입력과 일치할 시 출력
				{
					printw("%-16s | %-8d\n", curNode->name, curNode->score);
					check = 1; // 발견에 성공했으면 check를 1로 갱신
				}
				curNode = curNode->next;
			}
			if (check == 0) // rank.txt가 비어있지 않았지만 이름 탐색에 실패한 경우
				printw("Search failure: No name in the list\n");
		}
		else
			printw("Error: The list is empty\n");
	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if (ch == '3') {
		int num, index = 1;
		if (rankList != NULL)
		{
			echo();
			printw("Input the rank: ");
			scanw("%d", &num);
			if (1 <= num && num <= score_number)
			{ // 범위 내에 있을 때만 실행
				if (num == 1)
				{
					Node* delNode = rankList;
					rankList = rankList->next; // Head 삭제 시 다음 head 지정
					delNode->next = NULL; // 삭제할 Node의 다음 연결 끊기
					free(delNode); // 삭제 Node 해제
				}
				else
				{
					Node* delNode, * delPrevNode = rankList; // delPrevNode는 삭제 Node 직전의 Node
					for (index = 1; index < num - 1; index++)
						delPrevNode = delPrevNode->next;
					delNode = delPrevNode->next; // 삭제할 Node 특정하기
					delPrevNode->next = delNode->next; // delPrevNode의 연결을 삭제 Node 이후와 연결
					delNode->next = NULL; // 삭제할 Node의 다음 연결 끊기
					free(delNode); // 삭제 Node 해제
				}
				printw("Result: The rank is deleted\n");
				score_number--; // 전체 Node 개수 1 감소
				writeRankFile(); // 랭킹을 삭제했으면 갱신된 새로운 list로 파일에 입력
			}
			else // 범위를 벗어나면 에러 메세지 출력
				printw("Search failure: The rank is not in the list\n");
		}
		else
			printw("Error: The rank list is empty\n");
	}
	else
		return;
	noecho();
	getch();// 함수가 바로 종료되지 않게끔 키 입력 받을 때까지 기다리게 유도
	return;
}

void writeRankFile() {
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int sn = 0, i;
	//1. "rank.txt" 연다
	FILE* fp = fopen("rank.txt", "w");
	
	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", score_number);
	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	Node* start = rankList;
	if (sn == score_number) return; // score_number가 0이어서 list가 비어있는 경우 그냥 return
	else {
		Node* curNode = rankList;
		while (curNode != NULL)
		{
			fprintf(fp, "%s %d\n", curNode->name, curNode->score);
			curNode = curNode->next;
		}
	}
	fclose(fp);
}

void newRank(int score) {
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN + 1];
	int i, j;
	clear();
	//1. 사용자 이름을 입력받음
	echo();
	printw("Your name: ");
	getnstr(str, NAMELEN);
	noecho();
	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가 0일 경우와 아닐 때 구별
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->score = score;
	strcpy(newNode->name, str);
	newNode->next = NULL; // 새로운 노드 생성 및 정보 저장
	if (score_number != 0) {
		Node* curNode = rankList; // Head로 시작
		if (score > curNode->score)
		{ // 점수가 head보다 커서 맨 앞에 들어가는 경우
			newNode->next = curNode;
			rankList = newNode;
		}
		else
		{ // 중간 위치 탐색 과정
			while (curNode->next->score > score && curNode->next != NULL)
				curNode = curNode->next; // 삽입할 위치 확정
			newNode->next = curNode->next;
			curNode->next = newNode; // 연결 재배치
		}
	}
	else { // rankList가 비어있는 경우
		rankList = newNode;
	}
	score_number++;
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
