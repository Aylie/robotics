#include <stdlib.h>
#include <stdio.h>

float raw[12][6];
int obj_num=0;
int origin_map[5][5];

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

int robot_map[5][5];
int map(int x,int y, int map_index){
	if(x>4 || y>4 || x<0 || y<0){
		return 1;
	}
	
	if(map_index == 0)
		return origin_map[x][y];
	else if(map_index == 1)
		return robot_map[x][y];
}