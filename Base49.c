#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <math.h>
#include <string.h>


int write_file()
{
	char b49[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvw";
	int num = 0;
	FILE* f = fopen("Final.txt", "r");
	if(!f)
	{
		perror("Open");
		return -2;
	}

	int file = creat("file.txt", S_IREAD|S_IWRITE);
	if(file==0)
	{
		perror("Creat");
		return -2;
	}

	while((fscanf(f, "%d",&num)!=EOF))
	{
		write(file, &b49[num], 1);
	}


	fclose(f);
	close(file);


	if (remove("Final.txt") != 0)
	{
		perror("Error while file deleting!");
		return -1;
	}

	if (rename("file.txt", "Final.txt") != 0)
	{
		perror("Error while file rename!");
		return -1;
	}
}


int evclid_chertila_takoi_i_suda_zalez(int num)
{
	int file = creat("Final.txt", S_IREAD|S_IWRITE);
	if(file==0)
	{
		perror("Creat");
		return -2;
	}

	while(num > 0)
	{
		int i = 0, j = 0 ,t = 0, q = 0, n = 0, m = 0, nn = 0;
		char c;

		FILE* f = fopen("4.txt", "r");
		if(!f)
		{
			perror("Open");
			return -2;
		}

		int ff = creat("4.tmp", S_IREAD|S_IWRITE);
		if(ff==0)
		{
			perror("Creat");
			return -2;
		}

		while (fscanf(f, "%c", &c) == 1)
		{
			n++;
		}
		int* a = (int*)malloc(n * sizeof(int));
		int* b = (int*)malloc(n * sizeof(int));
		
		rewind(f);


		printf("Our num from file = ");
		while (fscanf(f, "%c", &c) == 1)
		{
			printf("%c", c);
			a[m]=c-'0';
			m++;//отвечает за количсество символов в нашем числе
		}
		printf("\n");

		for(i=0;i<m;i++)
		{
			b[i]=a[i];
		}
		printf("\n");


		i = 0;
		while(i<m-2)//считаем остаток
		{
			if((a[i]*10+a[i+1])>=49)
			{
				int t = (a[i]*10+a[i+1]) % 49;
				a[i]=(t-t%10)/10;
				a[i+1]=t%10;
				//printf("%d, %d\n", a[i], a[i+1]);
			}
			else if((a[i]*100+a[i+1]*10+a[i+2])>49)
			{
				int t = (a[i]*100+a[i+1]*10+a[i+2]) % 49;
				a[i]=0;
				a[i+1]=(t-t%10)/10;
				a[i+2]=t%10;
				//printf("%d, %d, %d\n", a[i], a[i+1], a[i+2]);
			}
			else
			{
				i++;
			}
		}

		printf("Num without remainder = ");
		for(i=m;i>=0;i--)
		{
			b[i]-=a[i];
		}
		for(i=m;i>=0;i--)
		{
			if(b[i]<0)
			{
				b[i]+=10;
				b[i-1]-=1;
			}
		}

		for(i=0;i<m;i++)
		{
			printf("%d", b[i]);
		}	

		i = m-1;
		while(i>0)
		{
			if(b[i]==0)
			{
				nn++;
				i--;
			}
			else
			{
				break;
			}
		}

		printf("\nQuantity of zeros in our numeric = %d\n\n", nn);


		i = 0;
		while(i<m-2)
		{
			if((b[i]*10+b[i+1])>=49)
			{
				int tt = ((b[i]*10+b[i+1])-((b[i]*10+b[i+1])%49))/49;
				printf("(%d + %d)/49 = %d\n\n",(b[i]*10+b[i+1])%49, (b[i]*10+b[i+1])-((b[i]*10+b[i+1])%49), tt);
				tt+='0';
				write(ff, &tt, 1);


				int t = (b[i]*10+b[i+1]) % 49;
				b[i]=(t-t%10)/10;
				b[i+1]=t%10;
				//printf("%d, %d\n", b[i], b[i+1]);
			}
			else if((b[i]*100+b[i+1]*10+b[i+2])>49)
			{
				int tt = ((b[i]*100+b[i+1]*10+b[i+2])-((b[i]*100+b[i+1]*10+b[i+2])%49))/49;
				printf("(%d + %d)/49 = %d\n\n",(b[i]*100+b[i+1]*10+b[i+2])%49, ((b[i]*100+b[i+1]*10+b[i+2])-((b[i]*100+b[i+1]*10+b[i+2])%49)), tt);
				tt+='0';
				write(ff, &tt, 1);
				


				int t = (b[i]*100+b[i+1]*10+b[i+2]) % 49;
				b[i]=0;
				b[i+1]=(t-t%10)/10;
				b[i+2]=t%10;
				//printf("%d, %d, %d\n", b[i], b[i+1], b[i+2]);
			}
			else
			{
				i++;
			}
		}
		for(i=0;i<nn;i++)
		{
			int ttt = 0 + '0';
			write(ff, &ttt, 1);
		}
		write(ff, "\n", 1);

		printf("Our remainder = ");
		for(i=0;i<m;i++)
		{
			printf("%d", a[i]);
		}
		printf("\n");


		fclose(f);
		close(ff);


		FILE* fff = fopen("4.tmp", "r");
		if(!f)
		{
			perror("Open");
			return -2;
		}

		int ffff = creat("5.txt", S_IREAD|S_IWRITE);
		if(ff==0)
		{
			perror("Creat");
			return -2;
		}

		int mmm = 0;
		while (fscanf(fff, "%c", &c) == 1)
		{
			mmm++;
		}
		mmm-=1;

		rewind(fff);

		for(i=0;i<m;i++)
		{
			b[i]=0;
		}
		

		while (fscanf(fff, "%c", &c) == 1&&mmm>0)
		{
			b[m-mmm]=c-'0';
			mmm--;
		}
		printf("\n");

		printf("Our devider = ");
		for(i=0;i<m;i++)
		{
			printf("%d", b[i]);
			b[i]+='0';
			write(ffff, &b[i], 1);
		}

		fclose(fff);
		close(ffff);

		printf("\n");
		if (remove("4.tmp") != 0)
		{
			perror("Error while file deleting!");
			return -1;
		}
		if (remove("4.txt") != 0)
		{
			perror("Error while file deleting!");
			return -1;
		}

		if (rename("5.txt", "4.txt") != 0)
		{
			perror("Error while file rename!");
			return -1;
		}
		
		printf("\n_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_\n");
		for(i = m-2;i<m;i++)
		{
			if(a[m-2]==0)
			{
				i++;
			}
			else
			{
				a[i]+='0';
				write(file, &a[i], 1);
			}
		}
		write(file, " ", 1);
		num--;
	}
	if (remove("4.txt") != 0)
	{
		perror("Error while file deleting!");
		return -1;
	}
	close(file);
}


int adding()
{
	int i, j ,t, q, n = 0, m = 0;
	char c;

	FILE* f = fopen("3.txt", "r");
	if(!f)
	{
		perror("Open");
		return -2;
	}

	int ff = creat("4.txt", S_IREAD|S_IWRITE);
	if(ff==0)
	{
		perror("Creat");
		return -2;
	}

	while (fscanf(f, "%c", &c) == 1 && c >='0' && c <='9')
	{
		n++;
	}
	n+=1;

	printf("%d symbols in mass!\n\n", n);

	printf("Our numerics which we gonna summ:\n");
	int* a = (int*)malloc(n * sizeof(int));
	int* b = (int*)malloc(n * sizeof(int));

	rewind(f);
	n = 0;
	//_-_-_-_-_-_-_считали 1 число вывели в массив_-_-_-_-_-_-_
	while (fscanf(f, "%c", &c) == 1 && c >='0' && c <='9')
	{
		printf("%c", c);
		a[m]=c-'0';
		m++;//отвечает за количсество символов в нашем числе
		n++;//отвечает за место в файле на котором мы остановились
	}
	printf("\n");
	//_-_-_-_-_-_-_считали 1 число вывели в массив_-_-_-_-_-_-_
	t = m;
	m = 0;
	n++;
	while(fscanf(f, "%c", &c) == 1/*&&c >='0' && c <='9'*/)
	{
		if(c == ' ')
		{
			printf("\n");
			m=0;
			n++;
			continue;
		}
		//_-_-_-_-_-_-_считали 2 число вывели в массив_-_-_-_-_-_-_
		printf("%c", c);
		b[m]=c-'0';
		a[m]+=b[m];
		n++;//отвечает за место в файле на котором мы остановились
		m++;
	}

	for(i=t-1;i>=0;i--)
	{
		if(a[i]>9)
		{
			int tmp = (a[i]-a[i]%10)/10;
			a[i]=a[i]%10;
			a[i-1]+=tmp;
		}
	}
	printf("\nOur summ:\n");

	for(i=0;i<t;i++)
	{
		printf("%d", a[i]);
		a[i]+='0';
		write(ff, &a[i], 1);
	}
	printf("\n\n");
	fclose(f);
	close(ff);
	if(remove("3.txt")!=0)
	{
		perror("Delete");
		return -1;
	}
	free(a);
	free(b);
}


int coding(int n)
{
	int i, j ,t, q;

	FILE* f = fopen("2.txt", "r");
	if(!f)
	{
		perror("Open");
		return -2;
	}

	int ff = creat("3.txt", S_IREAD|S_IWRITE);
	if(ff==0)
	{
		perror("Creat");
		return -2;
	}


	for(i=0;i<n;i++)
	{
		fscanf(f, "%d",&q);
		if(q<10)
		{
			int* a = (int*)malloc((8*n/2-n) * sizeof(int));
			a[0]=q;
			for(j=0;j<n;j++)
			{
				printf("%d", a[j]);
			}
			printf("\n");

			j=8*(n-(i+1));
			while(j>0)
			{
				for(t=(8*n/2-n)-1;t>=0;t--)
				{
					a[t]*=2;
				}

				for(t=(8*n/2)-1;t>=0;t--)
				{
					if(a[t]-9>0)
					{
						a[t]%=10;
						a[t-1]+=1;
					}
				}
				j--;
			}
			

			for(t=0;t<(8*n/2-n);t++)
			{
				printf("%d",a[t]);
				a[t]+='0';
				if(a[t]!='0')
				{
					write(ff, &a[t], 1);
				}
			}
			write(ff, " ", 1);
			printf("\n\n");
		}
		else if(q>9&&q<100)
		{
			int* a = (int*)malloc((8*n/2-n) * sizeof(int));
			a[(8*n/2-n)-2]=(q-(q%10))/10;
			a[(8*n/2-n)-1]=q%10;
			for(j=0;j<(8*n/2-n);j++)
			{
				printf("%d", a[j]);
			}
			printf("\n");

			j=8*(n-(i+1));
			while(j>0)
			{
				for(t=(8*n/2-n)-1;t>=0;t--)
				{
					a[t]*=2;
				}

				for(t=(8*n/2-n)-1;t>=0;t--)
				{
					if(a[t]-9>0)
					{
						a[t]%=10;
						a[t-1]+=1;
					}
				}
				j--;
			}
			

			for(t=0;t<(8*n/2-n);t++)
			{
				printf("%d",a[t]);
				a[t]+='0';
				write(ff, &a[t], 1);
			}
			write(ff, " ", 1);
			printf("\n\n");
		}
		else if(q>99&&q<1000)
		{
			int* a = (int*)malloc((8*n/2-n) * sizeof(int));
			a[(8*n/2-n)-3]=(q-(q%100))/100;
			a[(8*n/2-n)-2]=(q%100-(q%10))/10;
			a[(8*n/2-n)-1]=q%10;
			for(j=0;j<(8*n/2-n);j++)
			{
				printf("%d", a[j]);
			}
			printf("\n");

			j=8*(n-(i+1));
			while(j>0)
			{
				for(t=(8*n/2-n)-1;t>=0;t--)
				{
					a[t]*=2;
				}

				for(t=(8*n/2-n)-1;t>=0;t--)
				{
					if(a[t]-9>0)
					{
						a[t]%=10;
						a[t-1]+=1;
					}
				}
				j--;
			}
			

			for(t=0;t<(8*n/2-n);t++)
			{
				printf("%d",a[t]);
				a[t]+='0';
				write(ff, &a[t], 1);
			}
			write(ff, " ", 1);
			printf("\n\n");
		}
	}
	printf("\n");
	fclose(f);
	close(ff);
	if(remove("2.txt")!=0)
	{
		perror("Delete");
		return -1;
	}
}


int main(int argc, char* argv[])
{
	if ( argc != 2 )
	{
		printf ("Error!\nPlease use format <./a.out> <example.txt>\n");
		return -1;
	}

	int j = 0, n = 0, i = 0;
	char c;
	char b49[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvw";//ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/

	FILE* f = fopen(argv[1], "r");
	if(!f)
	{
		perror("Open");
		return -2;
	}


	//-_-_-_-_-_-_-_Cчитаем количество символов и создаём массив из чисел на основе этой информации-_-_-_-_-_-_-_
	while (fscanf(f, "%c", &c) == 1)
	{
		if(c=='a'&&c!='\0'&&c!='\n');
		{
			printf("%c", c);
			n++;
		}
	}
	n-=1;
	printf("\nn = %d\n\n", n);
	
	rewind(f);

	int ff = creat("2.txt", S_IREAD|S_IWRITE);
	if(ff<0)
	{
		perror("Creat");
		return -2;
	}

	

	while (fscanf(f, "%c", &c) ==  1 && j<n)
	{
        int a = c;
        printf("%c - ", c);
        printf("%d;", a);

        char* count;
		count = (char *)malloc(10 * sizeof(char));
		long long int v = 0; //количество цифр в числе n
		//разбиваем на отдельные символы число m[j]
		while (a > 9)
		{
			count[v++] = (a % 10) + '0';
			a = a / 10;
		}
		count[v++] = a + '0';
		count[v] = '\0';
		char t;
		//инвертируем массив символов
		for (int i = 0; i < v / 2; i++)
		{
			t = count[i];
			count[i] = count[v - 1 - i];
			count[v - 1 - i] = t;
		}
		v = 0;
		printf("m[%d] = ", j);
		while (count[v] != '\0')
		{
			write(ff, &count[v], 1);
			printf("%c", count[v++]);
		}
		write(ff, " ", 1);
		printf("\n");
		free(count);

        j++;
    }
	close(ff);

	coding(n);

	adding();

	evclid_chertila_takoi_i_suda_zalez(n);

	write_file();

    printf("\nOh hi Mark!\n");//проверка что программа прокрутилась до конца
	return 0;
}