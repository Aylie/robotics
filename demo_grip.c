#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


float raw[12][6];
int obj_num = 0;
int origin_map[5][5];
int box_map[3][5][5];
int robot_map[5][5];
char robot_rout[1000];
int strinc = 0;
int dot[3][2];
int box[3][2];
int dx,dy;

//picture to map
void picture_to_map(void)
{
	int i,j,x_cur,y_cur;	
	for(i=0;i<5;i++){
		for(j=0;j<5;j++){
			origin_map[i][j] = 0;
		}
	}
	float len,wide;
	FILE* fp = fopen("output1","r");
	fscanf(fp,"%f %f\n",&len,&wide);
	/* raw manual 
		0: pixel size
		1: x coordinate
		2: y coordinate
		3: orientation angle of object
		4: 1=destination or 2=blocks
		5: colours or 0=obsticle
	*/
	wide/=5;
	len /=5;
	while(fscanf(fp,"%f %f %f %f %f %f\n",&raw[obj_num][0],&raw[obj_num][1],&raw[obj_num][2],&raw[obj_num][3],&raw[obj_num][4],&raw[obj_num][5])!=EOF){
		x_cur =0;
		y_cur =0;
		while((y_cur*wide)<raw[obj_num][1]){
			y_cur++;
		}
		while((x_cur*wide)<raw[obj_num][2]){
			x_cur++;
		}
		if(raw[obj_num][5]==0){
			origin_map[x_cur-1][y_cur-1] = 2;
			obj_num++;
		}else if(raw[obj_num][5]==1){
			if(raw[obj_num][4]==1){
				origin_map[x_cur-1][y_cur-1] = 6;
			}else if(raw[obj_num][4]==2){
				origin_map[x_cur-1][y_cur-1] = 3;
			}
			obj_num++;
		}else if(raw[obj_num][5]==2){
			if(raw[obj_num][4]==1){
				origin_map[x_cur-1][y_cur-1] = 7;
			}else if(raw[obj_num][4]==2){
				origin_map[x_cur-1][y_cur-1] = 4;
			}
			
			obj_num++;
		}else if(raw[obj_num][5]==3){
			if(raw[obj_num][4]==1){
				origin_map[x_cur-1][y_cur-1] = 8;
			}else if(raw[obj_num][4]==2){
				origin_map[x_cur-1][y_cur-1] = 5;
			}
			obj_num++;
		}
		
	}
	//if((x < 0)||(x >= 5)||(y < 0)||(y >= 5))
		//return 1;
	//int m[5][5];	
	/*int m[5][5] = {0, 0, 0, 0, 0,
					 0, 2, 3, 0, 2,
					 0, 6, 0, 8, 0,
					 0, 5, 2, 0, 0,
					 0, 0, 4, 0, 7};*/
	//return m[x][y];
	
}

int map(int x,int y, int map_index){
	if(x>4 || y>4 || x<0 || y<0){
		return 1;
	}
	
	if(map_index == 0)
		return origin_map[x][y];
	else if(map_index == 1)
		return robot_map[x][y];
}


//create box rout map

void box_BFS_rout(int x,int y,int u,int p, int *map){

    int i=0,j=0,k=0,flag =0;
    map[x*5+y] = -3;
    map[u*5+p] = 0;
	
    while(flag == 0){
        for(j=0;j<5 && flag!=1;j++){
            for(k=0;k<5 && flag !=1;k++){
                if(map[j*5+k] == i){
                    if(j-1 >=0){
                        if(map[(j-1)*5+k] == -1){
                            map[(j-1)*5+k] = i+1;
                        }
                        if(map[(j-1)*5+k] == -3){
                            map[(j-1)*5+k] = i+1;
                            flag =1;
                            dx = j-1;
                            dy = k;
                        }
                    }if(j+1<5){
                        if(map[(j+1)*5+k] == -1){
                            map[(j+1)*5+k] = i+1;
                        }
                        if(map[(j+1)*5+k] == -3){
                            map[(j+1)*5+k] = i+1;
                            flag =1;
                            dx = j+1;
                            dy = k;
                        }
                    }if(k-1>=0){
                        if(map[(j)*5+k-1] == -1){
                            map[(j)*5+k-1] = i+1;
                        }
                        if(map[(j)*5+k-1] == -3){
                            map[(j)*5+k-1] = i+1;
                            flag =1;
                            dx = j;
                            dy = k-1;
                        }
                    }if(k+1<5){
                        if(map[(j)*5+k+1] == -1){
                            map[(j)*5+k+1] = i+1;
                        }
                        if(map[(j)*5+k+1] == -3){
                            map[(j)*5+k+1] = i+1;
                            flag =1;
                            dx = j;
                            dy = k+1;
                        }
                    }
                }
            }
        }
        i++;
    }   
}

void box_BFS_map(int number)
{
	int i, j;
	if(number == 1){
		for(i = 0; i < 5; i++)
			for(j = 0; j < 5; j++){
				if((origin_map[i][j] == 0)||(origin_map[i][j] > 5))
					box_map[number-1][i][j] = -1;
				else
				box_map[number-1][i][j] = -2;
			}
		box_BFS_rout(box[number-1][0], box[number-1][1], dot[number-1][0], dot[number-1][1], &box_map[number-1][0][0]);
		available_map(number);
	}
	else if(number == 2){
		for(i = 0; i < 5; i++)
			for(j = 0; j < 5; j++){
				if((origin_map[i][j] == 0)||(origin_map[i][j] > 6))
					box_map[number-1][i][j] = -1;
				else
				box_map[number-1][i][j] = -2;
			}
		
		box_BFS_rout(box[number-1][0], box[number-1][1], dot[number-1][0], dot[number-1][1], &box_map[number-1][0][0]);
		available_map(number);
	}
	else if(number == 3){
		for(i = 0; i < 5; i++)
			for(j = 0; j < 5; j++){
				if((origin_map[i][j] == 0)||(origin_map[i][j] > 7))
					box_map[number-1][i][j] = -1;
				else
				box_map[number-1][i][j] = -2;
			}
		
		box_BFS_rout(box[number-1][0], box[number-1][1], dot[number-1][0], dot[number-1][1], &box_map[number-1][0][0]);
		available_map(number);
	}
}


int available(int x, int y, int map_index) //0: X, 1: LR, 2: UD, 3:LRUD
{
	if((x < 0)||(x >= 5)||(y < 0)||(y >= 5))
		return 0;
	
	int flag = 0;
	if((map(x, y-1, map_index) != 2)&&(map(x, y-1, map_index) != 1)&&(map(x, y+1, map_index) != 2)&&(map(x, y+1, map_index) != 1))
		flag = 1;
	if((map(x-1, y, map_index) != 1)&&(map(x-1, y, map_index) != 2)&&(map(x+1, y, map_index) != 1)&&(map(x+1, y, map_index) != 2)){
		if(flag)
			return 3;
		else	
			return 2;
	}
	return flag;
}

available_map(int number)
{
	int i, x, y;
	x = box[number-1][0];
	y = box[number-1][1];
	int index = box_map[number-1][x][y];
	int availability = available(x, y, 0);
	
	//for(i = index; i > 0; i--){
		if(box_map[number-1][x+1][y] == index-1){
			if((availability != 2)&&(availability != 3))
				box_map[number-1][x+1][y] = -1;
		}
		if(box_map[number-1][x-1][y] == index-1){
			if((availability != 2)&&(availability != 3))
				box_map[number-1][x+1][y] = -1;
		}
		if(box_map[number-1][x][y-1] == index-1){
			if((availability != 1)&&(availability != 3))
				box_map[number-1][x+1][y] = -1;
		}
		if(box_map[number-1][x][y+1] == index-1){
			if((availability != 1)&&(availability != 3))
				box_map[number-1][x+1][y] = -1;
		}
	//}
}

//robot
int check_rout_value_map(int x, int y, int *rout_value_map)
{
	if((x < 0)||(x >= 5)||(y < 0)||(y >= 5))
		return -1;
		
	return rout_value_map[x*5 + y];
}

void initial_robot_map(void)
{
	int i, j;
	for(i = 0; i < 5; i++)
		for(j = 0; j < 5; j++){
			if((map(i, j, 0) == 0)||(map(i, j, 0) >= 6))
				robot_map[i][j] = -1; //set dot as free as well
			else	
				robot_map[i][j] = -2; //set box as obstacle as well
		}
}

void robot_go_to(int x,int y,int u,int p){
	//printf("robot go to %d, %d from %d, %d\n", u, p, x, y);
	
    int i=0,j=0,k=0,flag =0;
    char temp[100];
    robot_map[x][y] = -3;
    robot_map[u][p] = 0;
    while(flag == 0){
        for(j=0;j<5 && flag!=1;j++){
            for(k=0;k<5 && flag !=1;k++){
                if(robot_map[j][k] == i){
                    if(j-1 >= 0){
                        if(robot_map[j-1][k] == -1){
                            robot_map[j-1][k] = i+1;
                        }
                        if(robot_map[j-1][k] == -3){
                            robot_map[j-1][k] = i+1;
                            flag =1;
                            dx = j-1;
                            dy = k;
                        }
                    }if(j+1<5){
                        if(robot_map[j+1][k] == -1){
                            robot_map[j+1][k] = i+1;
                        }
                        if(robot_map[j+1][k] == -3){
                            robot_map[j+1][k] = i+1;
                            flag =1;
                            dx = j+1;
                            dy = k;
                        }
                    }if(k-1>=0){
                        if(robot_map[j][k-1] == -1){
                            robot_map[j][k-1] = i+1;
                        }
                        if(robot_map[j][k-1] == -3){
                            robot_map[j][k-1] = i+1;
                            flag =1;
                            dx = j;
                            dy = k-1;
                        }
                    }if(k+1<5){
                        if(robot_map[j][k+1] == -1){
                            robot_map[j][k+1] = i+1;
                        }
                        if(robot_map[j][k+1] == -3){
                            robot_map[j][k+1] = i+1;
                            flag =1;
                            dx = j;
                            dy = k+1;
                        }
                    }
                }
            }
        }
        i++;
    }
	//print_map(&robot_map[0][0]);
    j=0;
	dx = x;
	dy = y;
    while(1){
        if(i == 0){
            break;
        }
        if(robot_map[dx-1][dy] == i-1 && dx-1>=0){
            temp[j] = 'u';
            dx = dx-1;
            dy = dy;
            i--;
            j++;
            continue;
        }if(robot_map[dx+1][dy] == i-1 && dx+1 <5){
            temp[j] = 'd';
            dx = dx+1;
            dy = dy;
            i--;
            j++;
            continue;
        }if(robot_map[dx][dy-1] == i-1 && dy-1 >=0 ){
            temp[j] = 'l';
            dx = dx;
            dy = dy-1;
            i--;
            j++;
            continue;
        }if(robot_map[dx][dy+1] == i-1 && dy+1<5){
            temp[j] = 'r';
            dx = dx;
            dy = dy+1;
            i--;
            j++;
            continue;
        }
    }
    for(k = 0; k < j; k++){
        robot_rout[strinc] = temp[k];
		strinc++;
    }
	
	//print_string();
}

int initial_robot_to(int x, int y, int number)
{
	//printf("initial NO.%d map '%c' at %d, %d\n", number, robot_rout[strinc-2], x, y);
	// in;
	//for(in = 0; in < 3; in++)
		//printf("dot%d = %d, %d\n", in, dot[in][0], dot[in][1]);
	
	if(strinc == 0)
		robot_go_to(4, 4, x, y);
	else{
		switch(robot_rout[strinc-1]){
			case '2':
				robot_go_to(dot[number-2][0], dot[number-2][1]+1, x, y);
				break;
			case '4':
				robot_go_to(dot[number-2][0], dot[number-2][1]-1, x, y);
				break;
			case '6':
				robot_go_to(dot[number-2][0]+1, dot[number-2][1], x, y);
				break;
			case '8':
				robot_go_to(dot[number-2][0]-1, dot[number-2][1], x, y);
				break;
			default :
				robot_go_to(4, 4, x, y);
				break;
		} 
	}
	
	int i = robot_rout[strinc-1];
	if(i == 'l')
		return 1;
	else if(i == 'r')
		return 2;
	else if(i == 'u')
		return 3;
	else if(i == 'd')
		return 4;
	else
		return i;
}

int map_decoder( int number)
{
	//print_map(&box_map[number-1][0][0]);
	int *rout_map = &box_map[number-1][0][0];
	print_map(rout_map);
	char buffer;
	int i, x, y;
	x = box[number-1][0];
	y = box[number-1][1];
	//printf("box = %d, %d\n", x, y);
	
	int rout_value = check_rout_value_map(x, y, rout_map);
	//printf("rout_value = %d\n", rout_value);
	
	initial_robot_map();
	
	int a, b, c, d;
	if(number != 1){
		a = box[0][0];
		b = box[0][1];
		c = dot[0][0];
		d = dot[0][1];
		robot_map[a][b] = -1;
		robot_map[c][d] = -2;		
	}

	if(number == 3){
		a = box[1][0];
		b = box[1][1];
		c = dot[1][0];
		d = dot[1][1];
		robot_map[a][b] = -1;
		robot_map[c][d] = -2;		
	}
	//print_map(&robot_map[0][0]);	
	
	int turn;
	if(check_rout_value_map(x+1, y, rout_map) == rout_value-1){
		turn = initial_robot_to(x-1, y, number);
		/*
		if(turn == 1)
			buffer = '2';
		else if(turn == 2)
			buffer = '4';
		else if(turn == 3)
			buffer = '8';
		else if(turn == 4)
			buffer = '6';
		*/
		buffer = '8';
		robot_rout[strinc] = buffer;
		strinc++;
		
		buffer = 'd';
		robot_rout[strinc] = buffer;
		strinc++;
	}
	else if(check_rout_value_map(x-1, y, rout_map) == rout_value-1){
		turn = initial_robot_to(x+1, y, number);
		/*
		if(turn == 1)
			buffer = '4';
		else if(turn == 2)
			buffer = '2';
		else if(turn == 3)
			buffer = '6';
		else if(turn == 4)
			buffer = '8';
		*/
		buffer = '6';
		robot_rout[strinc] = buffer;
		strinc++;
		
		buffer = 'u';
		robot_rout[strinc] = buffer;
		strinc++;
	}
	else if(check_rout_value_map(x, y+1, rout_map) == rout_value-1){
		turn = initial_robot_to(x, y-1, number);
		/*
		if(turn == 1)
			buffer = '8';
		else if(turn == 2)
			buffer = '6';
		else if(turn == 3)
			buffer = '4';
		else if(turn == 4)
			buffer = '2';
		*/
		buffer = '4';
		robot_rout[strinc] = buffer;
		strinc++;
		
		buffer = 'r';
		robot_rout[strinc] = buffer;
		strinc++;
	}
	else if(check_rout_value_map(x, y-1, rout_map) == rout_value-1){
		turn = initial_robot_to(x, y+1, number);
		/*
		if(turn == 1)
			buffer = '6';
		else if(turn == 2)
			buffer = '8';
		else if(turn == 3)
			buffer = '2';
		else if(turn == 4)
			buffer = '4';
		*/
		buffer = '2';
		robot_rout[strinc] = buffer;
		strinc++;
		
		buffer = 'l';
		robot_rout[strinc] = buffer;
		strinc++;
	}
	
	char open_flag;
	for(i = rout_value; i > 0; i--){
		
		if(check_rout_value_map(x+1, y, rout_map) == rout_value-1){
			if(rout_value-1 != 0){
				buffer = 'd';
				robot_rout[strinc] = buffer;
				strinc++;
				
				x++;
				rout_value--;
				
				continue;
			}
			else	
				open_flag = 'd';

		}
		else if(check_rout_value_map(x-1, y, rout_map) == rout_value-1){
			if(rout_value-1 != 0){
				buffer = 'u';
				robot_rout[strinc] = buffer;
				strinc++;
				
				x--;
				rout_value--;
				
				continue;
			}
			else
				open_flag = 'u';
		}
		else if(check_rout_value_map(x, y+1, rout_map) == rout_value-1){
			if(rout_value-1 != 0){
				buffer = 'r';
				robot_rout[strinc] = buffer;
				strinc++;
				
				y++;
				rout_value--;
				
				continue;
			}
			else	
				open_flag = 'r';
		}
		else if(check_rout_value_map(x, y-1, rout_map) == rout_value-1){
			if(rout_value-1 != 0){
				buffer = 'l';
				robot_rout[strinc] = buffer;
				strinc++;
				
				y--;
				rout_value--;
				
				continue;
			}
			else
				open_flag = 'l';
		}
	}
	
	if(open_flag == 'l'){
		buffer = '2';
		robot_rout[strinc] = buffer;
		strinc++;
	}
	else if(open_flag == 'r'){
		buffer = '4';
		robot_rout[strinc] = buffer;
		strinc++;
	}
	else if(open_flag == 'u'){
		buffer = '6';
		robot_rout[strinc] = buffer;
		strinc++;
	}
	else if(open_flag == 'd'){
		buffer = '8';
		robot_rout[strinc] = buffer;
		strinc++;
	}
	/*
	char orien = robot_rout[strinc-1];
	switch(orien){
		case 'l':{
			if(open_flag == 'l'){
				buffer = '6';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'r'){
				buffer = '8';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'u'){
				buffer = '4';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'd'){
				buffer = '2';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			break;
		}
		case 'r':{
			if(open_flag == 'l'){
				buffer = '8';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'r'){
				buffer = '6';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'u'){
				buffer = '2';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'd'){
				buffer = '4';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			break;
		}
		case 'u':{
			if(open_flag == 'l'){
				buffer = '2';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'r'){
				buffer = '4';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'u'){
				buffer = '6';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'd'){
				buffer = '8';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			break;
		}
		case 'd':{
			if(open_flag == 'l'){
				buffer = '4';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'r'){
				buffer = '2';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'u'){
				buffer = '8';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			else if(open_flag == 'd'){
				buffer = '6';
				robot_rout[strinc] = buffer;
				strinc++;
			}
			break;
		}
		default:
			break;
	}
	*/
	return 0;
}


//print
void print_map(int *map)
{
	int i, j;
	printf("this is map :\n");
	for(i = 0; i < 5; i++){
		for(j = 0; j < 5; j++)
			printf(" %d ", map[i*5 + j]);
		printf("\n");
	}
	printf("\n");
}

void print_rout_map(int *rout_map)
{
	int i, j, k;
	for(i = 0; i < 3; i++){
		printf("this is No.%d rout map\n", i+1);
		for(j = 0; j < 5; j++){
			for(k = i*5*5 + j*5; k < i*5*5+j*5+5; k++)
				printf(" %d ", rout_map[k]);
			printf("\n");
		}
		printf("\n\n");
	}
	return;
}

void print_string(void)
{
	int i;
	printf("\n string = ");
	for(i = 0; i <strinc; i++){
		/*if(((cha[i]-'0') == 2)||((cha[i]-'0') == 4)||((cha[i]-'0') == 6)||((cha[i]-'0') == 8))
			printf("%d", cha[i]-'0');
		else*/
			printf("%c", robot_rout[i]);
	}
	printf("\n");
}

// testbench below
int main(void)
{
	int i, j, k;
	bzero(robot_rout,sizeof(robot_rout));
	picture_to_map();
	//print_map(&origin_map[0][0]);
	

	for(i = 0; i < 5; i++)
		for(j = 0; j < 5; j++){
			if(map(i, j, 0) == 3){
				box[0][0] = i;
				box[0][1] = j;
				//printf("box1 = %d, %d\n", i, j);
			}
			else if(map(i, j, 0) == 4){
				box[1][0] = i;
				box[1][1] = j;
				//printf("box2 = %d, %d\n", i, j);
			}
			else if(map(i, j, 0) == 5){
				box[2][0] = i;
				box[2][1] = j;
				//printf("box1 = %d, %d\n", i, j);
			}
			else if(map(i, j, 0) == 6){
				dot[0][0] = i;
				dot[0][1] = j;
				//printf("dot1 = %d, %d\n", i, j);
			}
			else if(map(i, j, 0) == 7){
				dot[1][0] = i;
				dot[1][1] = j;
				//printf("dot2 = %d, %d\n", i, j);
			}
			else if(map(i, j, 0) == 8){
				dot[2][0] = i;
				dot[2][1] = j;
				//printf("dot3 = %d, %d\n", i, j);
			}
		}

		
	for(i = 1; i <= 3; i++){
		box_BFS_map(i);
	}
	
	//print_rout_map(&box_map[0][0][0]);
	
	for(i = 1; i <= 3; i++){
		map_decoder(i);
		//print_string();
	}
	
	print_map(&origin_map[0][0]);
	print_string();

	return 0;
}

