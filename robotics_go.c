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

#include<stdio.h>

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

int go(int x, int y, int orien, int *rout_map, int *dirty_map, int number)
{
	//printf("enter go from (%d, %d), orien = %d\n", x, y, orien);
	int object, rout = 0;
	
	if((x < 0)||(x >= 5)||(y < 0)||(y >= 5))
		return 0;
	
	int dirty_value =  check_dirty_map(x, y, orien, dirty_map);
	if(dirty_value != -1){
		//printf("dirty!! (%d, %d) is %d\n", x, y, dirty_value);
		return dirty_value;
	}
	
	
	object = map(x, y);
	if((object == 1) || (object == 2)){
		rout = 0;
		//printf("rout %d, %d = %d obstacle\n", x, y, rout);
		set_dirty_map(x, y, orien, dirty_map, rout);
		return rout;
	}
	
	if(orien){
		int available_result = available(x, y);
		//printf("available result = %d, orien = %d\n", available_result, orien);
		if(available_result != 3){
			if((orien == 1)||(orien == 2)){
				if(available_result != 1){
					rout = 0;
					//printf("rout %d, %d = %d orien\n", x, y, rout);
					set_dirty_map(x, y, orien, dirty_map, rout);
					return rout;
				}
			}
			
			else if((orien == 3)||(orien == 4)){
				if(available_result != 2){
					rout = 0;
					//printf("rout %d, %d = %d orien\n", x, y, rout);
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
			//printf("rout %d, %d = %d box1\n", x, y, rout);
			return rout;
		}
	}
	else if(object == 4){
		rout += 5;
		if(number == 2){
			save_rout(x, y, rout, rout_map);
			set_dirty_map(x, y, orien, dirty_map, rout);
			//printf("rout %d, %d = %d box1\n", x, y, rout);
			return rout;
		}
	}
	else if(object == 5){
		rout += 9;
		if(number == 3){
			save_rout(x, y, rout, rout_map);
			set_dirty_map(x, y, orien, dirty_map, rout);
			//printf("rout %d, %d = %d box1\n", x, y, rout);
			return rout;
		}
	}
	
	//0: X, 1: L, 2: R, 3: U, 4: D
	switch(orien){
		case 1:
			rout += go(x, y+1, 1, rout_map, dirty_map, number) + go(x+1, y, 3, rout_map, dirty_map, number) + go(x-1, y, 4, rout_map, dirty_map, number);
			break;
		case 2:
			rout += go(x, y-1, 2, rout_map, dirty_map, number) + go(x+1, y, 3, rout_map, dirty_map, number) + go(x-1, y, 4, rout_map, dirty_map, number);
			break;
		case 3:
			rout += go(x+1, y, 3, rout_map, dirty_map, number) + go(x, y-1, 2, rout_map, dirty_map, number) + go(x, y+1, 1, rout_map, dirty_map, number);
			break;
		case 4:
			rout += go(x-1, y, 4, rout_map, dirty_map, number) + go(x, y-1, 2, rout_map, dirty_map, number) + go(x, y+1, 1, rout_map, dirty_map, number);
			break;
		default:
			rout += go(x+1, y, 3, rout_map, dirty_map, number) + go(x-1, y, 4, rout_map, dirty_map, number) + go(x, y+1, 1, rout_map, dirty_map, number) + go(x, y-1, 2, rout_map, dirty_map, number);
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
	//printf("rout %d, %d = %d return\n", x, y, rout);
	return rout;	
}

//int rout_map[5][5];

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

void save_rout(int x, int y, int rout, int* rout_map) //0: X, 1~4: available rout, need to move box1, 5~8: available rout, need to move box2, 9~12: available rout, need to move box3
{
	*(rout_map + (x*5 + y)) = rout;
	//printf("save %d, %d as %d\n", x, y, rout_map[x*5 + y]);
	return;
}

int available(int x, int y) //0: X, 1: LR, 2: UD, 3:LRUD
{
	if((x < 0)||(x >= 5)||(y < 0)||(y >= 5))
		return 0;
	
	int flag = 0;
	if((map(x, y-1) != 2)&&(map(x, y-1) != 1)&&(map(x, y+1) != 2)&&(map(x, y+1) != 1))
		flag = 1;
	if((map(x-1, y) != 1)&&(map(x-1, y) != 2)&&(map(x+1, y) != 1)&&(map(x+1, y) != 2)){
		if(flag)
			return 3;
		else	
			return 2;
	}
	return flag;
}

// testbench below



int map(int x, int y)
{
	if((x < 0)||(x >= 5)||(y < 0)||(y >= 5))
		return 1;
		
	int map[5][5] = {0, 0, 0, 0, 0,
					 0, 2, 0, 0, 2,
					 0, 6, 3, 7, 0,
					 0, 4, 2, 0, 0,
					 8, 0, 5, 0, 0};
	return map[x][y];
}

void print_map(void)
{
	int i, j;
	printf("this is map :\n");
	for(i = 0; i < 5; i++){
		for(j = 0; j < 5; j++)
			printf(" %d ", map(i, j));
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

int main(void)
{
	int i, j;
	int dot[3][2];
	
	print_map();
	
	for(i = 0; i < 5; i++)
		for(j = 0; j < 5; j++){
			if(map(i, j) == 6){
				dot[0][0] = i;
				dot[0][1] = j;
				//printf("dot1 = %d, %d\n", i, j);
			}
			else if(map(i, j) == 7){
				dot[1][0] = i;
				dot[1][1] = j;
				//printf("dot2 = %d, %d\n", i, j);
			}
			else if(map(i, j) == 8){
				dot[2][0] = i;
				dot[2][1] = j;
				//printf("dot3 = %d, %d\n", i, j);
			}
		}

	
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
		
		go(dot[i][0], dot[i][1], 0, &rout_map[i][0][0], &dirty_map[0][0][0], i+1);
		//printf("\nfinish %d go\n", i+1);
		//print_rout_map(&rout_map[0][0][0]);
	}
	
	print_rout_map(&rout_map[0][0][0]);
	
	return 0;
}