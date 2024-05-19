#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INF 10001

typedef struct element{
	int direct;
	int cost;
	int next;
}element;

int node;
struct element table[100][100];
char message[100][1001]={0,};

void print_table(FILE *output_file) {
    int a = 0;
    while (a < node) {
        int i = 0;
        while (i < node) {
            int cost = table[a][i].cost;
            if (cost != INF) {
                fprintf(output_file, "%d %d %d\n", i, table[a][i].next, cost);
            }
            i++;
        }
        fprintf(output_file, "\n");
        a++;
    }
}


void transMsg(FILE *output_file, int cnt){
	int start, dest, now, temp; 
	for(int i=0;i<cnt;i++){
		start= message[i][0]-'0';
		dest= message[i][2]-'0';

        if (table[start][dest].cost == INF) {
            fprintf(output_file, "from %d to %d cost infinite hops unreachable ", start, dest);
            temp = 4;
        } else {
            fprintf(output_file, "from %d to %d cost %d hops %d ", start, dest, table[start][dest].cost, start);
            now = table[start][dest].next;
            for (; now != dest; now = table[now][dest].next) {
                fprintf(output_file, "%d ", now);
            }
            temp = 4;
        }

		fprintf(output_file,"message ");
        for (; message[i][temp] != '\0'; ++temp) {
            fprintf(output_file, "%c", message[i][temp]);
        }
	}
	if(cnt) fprintf(output_file,"\n");
	
};

int SPT[100][100] = {-1};

void dijkstra(int start){
	int P[100];
	int visited[100];
	memset(visited, 0, sizeof(visited));
	memset(SPT[start], -1, sizeof(SPT[start]));
	memset(P, -1, sizeof(P));

    for (int i = 0; i < node; i++) {
    P[i] = (table[start][i].direct < INF && start != i) ? start : P[i];
    table[start][i].next = (table[start][i].direct < INF && start != i) ? i : table[start][i].next;
}

	SPT[start][0] = start;
	P[start] = -1;
	visited[start] = 1;
    
	for (int i = 1; i < node; i++){

        int min = INF;
        int idx = -1;
        for (int j = 0; j < node; j++) {
            if (P[j] == -1 || visited[j] != 0) {
                continue;
            }
            
            int cost = table[start][j].cost;
            if (cost < min) {
                min = cost;
                idx = j;
            }
        }


		if (idx ==-1)break;
		SPT[start][i] = idx;
		visited[idx] = 1;
		table[start][idx].cost = min;

		int j = idx;
		while (P[j] != start) j = P[j];
		table[start][idx].next = j;

        for (int j = 0; j < node; j++) {
        if (visited[j] == 0) {
        int new_cost = table[idx][j].direct + table[start][idx].cost;
        
        if (table[start][j].cost > new_cost) {
            table[start][j].cost = new_cost;
            P[j] = idx;
        }
        else if (table[start][j].cost == new_cost && idx < P[j]) {
            P[j] = idx;
        }
    }
}

	}


}

void Routing() {
    int i = 0;
    while (i < node) {
        dijkstra(i);
        i++;
    }
}


void input_Table(int a,int b, int c){
	table[a][b].cost = c;
	table[b][a].cost = c;
	table[a][b].direct = c;
	table[b][a].direct = c;
	table[a][b].next = b;
	table[b][a].next = a;
}

void get_change(int a, int b, int c){
    for (int i = 0; i < node; i++) {
    for (int j = 0; j < node; j++) {
        table[i][j].cost = (table[i][j].direct > 0) ? table[i][j].direct : ((i == j) ? 0 : INF);
        table[i][j].next = (table[i][j].direct > 0) ? j : ((i == j) ? j : -1);
    }
}
	if(c==-999){
		table[a][b].cost = table[b][a].cost = INF;
		table[a][b].direct = table[b][a].direct = INF;
		table[a][b].next = table[b][a].next = -1;		
	}
	else{
		input_Table(a,b,c);
	}
}

int main(int argc, char *argv[]) {
	
    FILE *top_file = fopen(argv[1], "r");//topologyfile
	FILE *mes_file = fopen(argv[2], "r");//messagesfile
	FILE *cha_file = fopen(argv[3], "r");//changesfile

    //(3) 인자의 수가 맞지 않으면, 에러 메세지 출력하고 프로그램 종료
	if(argc!=4){
		fprintf(stderr, "usage: linkstate topologyfile messagesfile changesfile\n");
		exit(1);
	}
    //(4) input file open 에러가 발생하면 에러 메세지 출력하고 프로그램 종료
	if ((top_file == NULL)||(mes_file == NULL)||(cha_file == NULL)){
		fprintf(stderr, "Error: open input file.\n");
		exit(1);
	}

    //output file 열기
	FILE *output_file = fopen("output_ls.txt", "w");
    
    //topoloyfile에서 정수 값을 읽어와 node에 저장
	int node1, node2, cost; 
	fscanf(top_file, "%d", &node);

    int i, j;
    for (i = 0; i < node; i++) {
        for (j = 0; j < node; j++) {
            if (i != j) {
                table[i][j].direct = INF;
                table[j][i].direct = INF;
                table[i][j].cost = INF;
                table[j][i].cost = INF;
                table[i][j].next = -1;
                table[j][i].next = -1;
            } else {
                table[i][j].cost = 0;
                table[i][j].next = j;
            }

        }
    }

    for (;;) {
    fscanf(top_file, "%d %d %d", &node1, &node2, &cost);
    if (feof(top_file))
        break;
    input_Table(node1, node2, cost);
}

	Routing();
	print_table(output_file);

    int cnt = 0;
    bool reachedEOF = false;
    while (!reachedEOF) {
        fgets(message[cnt++], sizeof(message[cnt]), mes_file);
        reachedEOF = feof(mes_file);
    }

	transMsg(output_file,cnt-1);
	
    	while (fscanf(cha_file, "%d %d %d", &node1, &node2, &cost) != EOF) {
        get_change(node1, node2, cost);
        Routing();
        print_table(output_file);
        transMsg(output_file, cnt - 1);
    }
	
	printf("Complete. Output file written to output_ls.txt.\n");

    //file 닫기
	fclose(top_file);
	fclose(cha_file);
	fclose(mes_file);
	fclose(output_file);
	return 0;
}
