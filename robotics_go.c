////////////////////////////////////////////////////////
//available  0: X, 1: LR, 2: UD, 3:LRUD               //
//orien      0: X, 1: L, 2: R, 3: U, 4: D             //
//go         0: X, 1: V                               //
//map        0: free, 1: wall, 2: obstacle, 3: box1   //
//           4: box2, 5: box3, 6: dot1, 7: dot2       //
//           8: dot3                                  //
//save_rout  0: X,                                    // 
//           1~4: available rout, need to move box1   //
//           5~8: available rout, need to move box2   //
//			 9~12: available rout, need to move box3  //
////////////////////////////////////////////////////////

//define #BOX, #BOX2, #BOX3

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

float raw[12][6];
int obj_num=0;
int origin_map[5][5];
int robot_map[5][5];


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
	FILE* fp = fopen("output1.txt","r");
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
		}else if(raw[obj_num][5]==1){
			if(raw[obj_num][4]==1){
				origin_map[x_cur-1][y_cur-1] = 6;
			}else if(raw[obj_num][4]==2){
				origin_map[x_cur-1][y_cur-1] = 3;
			}
		}else if(raw[obj_num][5]==2){
			if(raw[obj_num][4]==1){
				origin_map[x_cur-1][y_cur-1] = 7;
			}else if(raw[obj_num][4]==2){
				origin_map[x_cur-1][y_cur-1] = 4;
			}
		}else if(raw[obj_num][5]==3){
			if(raw[obj_num][4]==1){
				origin_map[x_cur-1][y_cur-1] = 8;
			}else if(raw[obj_num][4]==2){
				origin_map[x_cur-1][y_cur-1] = 5;
			}
		}
		obj_num++;
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


//creat box_rout_map
int check_dirty_map(int x, int y, int orien, int *dirty_map)
{
	/*
	int flag;
	if((orien == 1)||(orien == 2))
		flag = 0;
	else if((orien == 3)||(orien == 4))
		flag = 1;
	*/
	if(orien == 0)
		return -1;
	//printf("check dirty (%d, %d) with orien = %d\n", x, y, orien);
	return dirty_map[(orien-1)*5*5 + x*5 + y];		
}

void set_dirty_map(int x, int y, int orien, int *dirty_map, int value)
{
	/*
	int flag;
	if((orien == 1)||(orien == 2))
		flag = 0;
	else if((orien == 3)||(orien == 4))
		flag = 1;
	*/
	
	dirty_map[(orien-1)*5*5 + x*5 + y] = value;
}

void save_rout(int x, int y, int rout, int* rout_map) //0: X, 1~4: available rout, need to move box1, 5~8: available rout, need to move box2, 9~12: available rout, need to move box3
{
	*(rout_map + (x*5 + y)) = rout;
	//printf("save %d, %d as %d\n", x, y, rout_map[x*5 + y]);
	return;
}

int temp[5][5];
int go(int x, int y, int orien, int *rout_map, int *dirty_map, int number, int map_index)
{
	printf("enter go from (%d, %d), orien = %d\n", x, y, orien);
	int object, rout = 0;
	
	if((x < 0)||(x >= 5)||(y < 0)||(y >= 5))
		return 0;

	if(temp[x][y] == 1)
		return 0;
	else
		temp[x][y] = 1;
		
	int dirty_value =  check_dirty_map(x, y, orien, dirty_map);
	if(dirty_value != -1){
		//printf("dirty!! (%d, %d) is %d\n", x, y, dirty_value);
		return dirty_value;
	}
	
	
	object = map(x, y, map_index);
	if((object == 1) || (object == 2)){
		rout = 0;
		printf("rout %d, %d = %d obstacle\n", x, y, rout);
		set_dirty_map(x, y, orien, dirty_map, rout);
		return rout;
	}
	
	if(orien){
		int available_result = available(x, y, map_index);
		//printf("available result = %d, orien = %d\n", available_result, orien);
		if(available_result != 3){
			if((orien == 1)||(orien == 2)){
				if(available_result != 1){
					rout = 0;
					printf("rout %d, %d = %d orien\n", x, y, rout);
					set_dirty_map(x, y, orien, dirty_map, rout);
					return rout;
				}
			}
			
			else if((orien == 3)||(orien == 4)){
				if(available_result != 2){
					rout = 0;
					printf("rout %d, %d = %d orien\n", x, y, rout);
					set_dirty_map(x, y, orien, dirty_map, rout);
					return rout;
				}
			}
		}
	}
	
	if(object == 3){
		rout += 1;
		if(number == 1){
			save_rout(x, y, rout, rout_map);
			set_dirty_map(x, y, orien, dirty_map, rout);
			printf("rout %d, %d = %d box1\n", x, y, rout);
			return rout;
		}
	}
	else if(object == 4){
		rout += 5;
		if(number == 2){
			save_rout(x, y, rout, rout_map);
			set_dirty_map(x, y, orien, dirty_map, rout);
			printf("rout %d, %d = %d box2\n", x, y, rout);
			return rout;
		}
	}
	else if(object == 5){
		rout += 9;
		if(number == 3){
			save_rout(x, y, rout, rout_map);
			set_dirty_map(x, y, orien, dirty_map, rout);
			printf("rout %d, %d = %d box3\n", x, y, rout);
			return rout;
		}
	}
	
	//0: X, 1: L, 2: R, 3: U, 4: D
	switch(orien){
		case 1:
			rout += go(x, y+1, 1, rout_map, dirty_map, number, map_index) + go(x+1, y, 3, rout_map, dirty_map, number, map_index) + go(x-1, y, 4, rout_map, dirty_map, number, map_index);
			break;
		case 2:
			rout += go(x, y-1, 2, rout_map, dirty_map, number, map_index) + go(x+1, y, 3, rout_map, dirty_map, number, map_index) + go(x-1, y, 4, rout_map, dirty_map, number, map_index);
			break;
		case 3:
			rout += go(x+1, y, 3, rout_map, dirty_map, number, map_index) + go(x, y-1, 2, rout_map, dirty_map, number, map_index) + go(x, y+1, 1, rout_map, dirty_map, number, map_index);
			break;
		case 4:
			rout += go(x-1, y, 4, rout_map, dirty_map, number, map_index) + go(x, y-1, 2, rout_map, dirty_map, number, map_index) + go(x, y+1, 1, rout_map, dirty_map, number, map_index);
			break;
		default:
			rout += go(x+1, y, 3, rout_map, dirty_map, number, map_index) + go(x-1, y, 4, rout_map, dirty_map, number, map_index) + go(x, y+1, 1, rout_map, dirty_map, number, map_index) + go(x, y-1, 2, rout_map, dirty_map, number, map_index);
			break;
	}
	
	int flag = 0;
	switch(number){
		case 1:{
			if((rout == 1)||(rout == 6)||(rout == 10)||(rout > 14))
				flag = 1;
			break;
		}
		case 2:{
			if((rout == 5)||(rout == 6)||(rout == 14)||(rout > 14))
				flag = 1;
			break;
		}
		case 3:{
			if((rout == 9)||(rout == 10)||(rout == 14)||(rout > 14))
				flag = 1;
			break;
		}
		default :
			flag = 0;
			break;
	}
	
	if(flag == 0)
		rout = 0;
	save_rout(x, y, rout, rout_map);
	set_dirty_map(x, y, orien, dirty_map, rout);
	printf("rout %d, %d = %d return\n", x, y, rout);
	temp[x][y] = 0;
	return rout;	
}


int initial_rout_map(int *rout_map) // initial to 0
{
	int i = 0;
	for(i = 0; i < 3*5*5; i++)
		rout_map[i] = 0;
		
	return i;
}

int initial_dirty_map(int *dirty_map) //initial to -1
{
	int i = 0;
	for(i = 0; i < 4*5*5; i++)
		dirty_map[i] = -1;
	
	return i;
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



//map decoder

int check_rout_value_map(int x, int y, int *rout_value_map)
{
	if((x < 0)||(x >= 5)||(y < 0)||(y >= 5))
		return 0;
		
	return rout_value_map[x*5 + y];
}

int check_rout_value(int rout_value, int number)
{
	if(rout_value == 0)
		return 0;
		
	int flag;
	switch(number){
		case 1:{
			if((rout_value == 1)||(rout_value == 6)||(rout_value == 10)||(rout_value > 14))
				flag = 1;
			break;
		}
		case 2:{
			if((rout_value == 5)||(rout_value == 6)||(rout_value == 14)||(rout_value > 14))
				flag = 1;
			break;
		}
		case 3:{
			if((rout_value == 9)||(rout_value == 10)||(rout_value == 14)||(rout_value > 14))
				flag = 1;
			break;
		}
		default :
			flag = 0;
			break;
	}
	
	return flag;
}


void initial_robot_map(void)
{
	int i, j;
	for(i = 0; i < 5; i++)
		for(j = 0; j < 5; j++){
			if((map(i, j, 0) == 0)||(map(i, j, 0) >= 6))
				robot_map[i][j] = 0; //set dot as free as well
			else	
				robot_map[i][j] = 2; //set box as obstacle as well
		}
}

void set_robot_map(int x, int y, int value)
{
	printf("set %d, %d as %d\n", x, y, value);
	print_map(&robot_map[0][0]);
	
	robot_map[x][y] = value;
}

//0: X, 1: L, 2: R, 3: U, 4: D
void robot_map_decoder(int now_x, int now_y, int go_x, int go_y, int fd)
{
	char buffer;
	if(go_x > now_x){
		if(robot_map[now_x+1][now_y]){
			//do down
			buffer = 4;
			write(fd, &buffer, 1);
			
			robot_map_decoder(now_x+1, now_y, go_x, go_y, fd);
		}
	}
	
	else{
		if(robot_map[now_x-1][now_y]){
			//do up
			buffer = 3;
			write(fd, &buffer, 1);
			
			robot_map_decoder(now_x-1, now_y, go_x, go_y, fd);
		}
	}
	
	if(go_y > now_y){
		if(robot_map[now_x][now_y+1]){
			//do right
			buffer = 2;
			write(fd, &buffer, 1);
			
			robot_map_decoder(now_x, now_y+1, go_x, go_y, fd);
		}
	}
	
	else{
		if(robot_map[now_x-1][now_y]){
			//do left
			buffer = 1;
			write(fd, &buffer, 1);
			
			robot_map_decoder(now_x, now_y-1, go_x, go_y, fd);
		}
	}
}

//0: X, 1: V, 2: you are here
int robot_go_to(int now_x, int now_y, int go_x, int go_y, int fd)
{
	printf("robot go from %d, %d to %d, %d\n", now_x, now_y, go_x, go_y);
	if((now_x == go_x)&&(now_y == go_y))
		return 2;
	
	int robot_dirty_map[4][5][5];
	int flag;
	flag = initial_dirty_map(&robot_dirty_map[0][0][0]);
	if(flag == 0)
		printf("initial dirty map erro\n");
	
	robot_map[now_x][now_y] = 3;
	
	int robot_rout_map[5][5];
	int i, j;
	for(i = 0; i < 5; i++)
		for(j = 0; j < 5; j++)
			robot_rout_map[i][j] = 0;
	
	flag = go(go_x, go_y, 0, &robot_rout_map[0][0], &robot_dirty_map[0][0][0], 1, 1);
	print_map(robot_rout_map);
	
	
	if(flag)
		robot_map_decoder(now_x, now_y, go_x, go_y, fd);
	
	return flag;
}

//0: X, 1: L, 2: R, 3: U, 4: D, 5: grab, 6: open
int rout_value_map_decoder(int x, int y, int orien, int *dot, int number, int *rout_value_map, int fd) //dot (x, y) = (dot[0], dot[y])
{
	printf("enter rout map decorder\n");

	int rout_value;
	char buffer;
	
	int robot_flag = 0, orien_flag = 0;
	
	if((x == dot[0])||(y == dot[1])){
		buffer = 6;
		write(fd, &buffer, 1);
		return 1;
	}
	
	if(dot[0] > x){
		rout_value = check_rout_value_map(x+1, y, rout_value_map);
	
		if(check_rout_value(rout_value, number)){
			if(orien != 4)
				orien_flag = 1;
			
			if(orien_flag){
				buffer = 6;
				write(fd, &buffer, 1);
				//open if need to turn
			}
			
			robot_flag = robot_go_to(x, y, x-1, y, fd); //turn to box
			if(robot_flag == 0)
				printf("robot can't go\n");
			
			if(orien_flag){
				buffer = 5;
				write(fd, &buffer, 1);
				//grab if need to turn
			}
			
			buffer = 4;
			write(fd, &buffer, 1);
			
			set_robot_map(x, y, 0);
			set_robot_map(x+1, y, 2);
			
			//go down
			return (rout_value_map_decoder(x+1, y, 4, dot, number, rout_value_map, fd));			
		}
	}
	
	else{
		rout_value = check_rout_value_map(x-1, y, rout_value_map);
	
		if(check_rout_value(rout_value, number)){
			if(orien != 3)
				orien_flag = 1;
			
			if(orien_flag){
				buffer = 6;
				write(fd, &buffer, 1);
				//open if need to turn
			}
			
			robot_flag = robot_go_to(x, y, x+1, y, fd); //turn to box
			if(robot_flag == 0)
				printf("robot can't go\n");
			
			if(orien_flag){
				buffer = 5;
				write(fd, &buffer, 1);
				//grab if need to turn
			}
			
			buffer = 3;
			write(fd, &buffer, 1);
			
			set_robot_map(x, y, 0);
			set_robot_map(x-1, y, 2);
			
			//go up
			return (rout_value_map_decoder(x-1, y, 3, dot, number, rout_value_map, fd));
		}
	}
	
	if(dot[1] > y){
		rout_value = check_rout_value_map(x, y+1, rout_value_map);
	
		if(check_rout_value(rout_value, number)){
			if(orien != 2)
				orien_flag = 1;
			
			if(orien_flag){
				buffer = 6;
				write(fd, &buffer, 1);
				//open if need to turn
			}
			
			robot_flag = robot_go_to(x, y, x, y-1, fd); //turn to box
			if(robot_flag == 0)
				printf("robot can't go\n");
			
			if(orien_flag){
				buffer = 5;
				write(fd, &buffer, 1);
				//grab if need to turn
			}
			
			buffer = 2;
			write(fd, &buffer, 1);
			
			set_robot_map(x, y, 0);
			set_robot_map(x, y+1, 2);
			
			//go right
			return (rout_value_map_decoder(x, y+1, 2, dot, number, rout_value_map, fd));
		}
	}
	
	else{
		rout_value = check_rout_value_map(x, y-1, rout_value_map);
	
		if(check_rout_value(rout_value, number)){
			if(orien != 1)
				orien_flag = 1;
			
			if(orien_flag){
				buffer = 6;
				write(fd, &buffer, 1);
				//open if need to turn
			}
			
			robot_flag = robot_go_to(x, y, x-1, y, fd); //turn to box
			if(robot_flag == 0)
				printf("robot can't go\n");
			
			if(orien_flag){
				buffer = 5;
				write(fd, &buffer, 1);
				//grab if need to turn
			}
			
			buffer = 1;
			write(fd, &buffer, 1);
			
			set_robot_map(x, y, 0);
			set_robot_map(x, y-1, 2);
			
			//go left
			return (rout_value_map_decoder(x, y-1, 1, dot, number, rout_value_map, fd));
		}
	}
	
	return 0;
}


// testbench below
int main(void)
{
	int i, j;
	int box[3][2];
	int dot[3][2];
	
	picture_to_map();
	print_map(&origin_map[0][0]);
	
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

	for(i = 0; i < 5; i++)
		for(j = 0; j < 5; j++)
			temp[i][j] = 0;

	int rout_map[3][5][5];
	int dirty_map[4][5][5];
	
	int flag;
	
	flag = initial_rout_map(&rout_map[0][0][0]);
	if(flag == 0)
		printf("initial rout map erro\n");
		
	for(i = 0; i < 3; i++){
		
		flag = initial_dirty_map(&dirty_map[0][0][0]);
		if(flag == 0)
			printf("initial dirty map erro\n");
		
		go(dot[i][0], dot[i][1], 0, &rout_map[i][0][0], &dirty_map[0][0][0], i+1, 0);
		printf("\nfinish %d go\n", i+1);
		print_rout_map(&rout_map[0][0][0]);
	}
	
	print_map(&origin_map[0][0]);
	//print_rout_map(&rout_map[0][0][0]);
	
	//decorder testbench below
	int fd = open("robot_rout.txt", O_RDWR|O_APPEND);
	
	initial_robot_map();
	print_map(&robot_map[0][0]);

	for(i = 0; i < 3; i++){
		rout_value_map_decoder(box[i][0], box[i][1], 0, &dot[i][0], i, &rout_map[i][0][0], fd);
	}
	
	close(fd);
	return 0;
}
