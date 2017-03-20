#include <stdio.h>
#include <math.h>

int power;
float *tabwspf;
float tabwspft[4];

float borderA;
float borderB;
int quantityP;

float *tabX;
float *tabY;

typedef struct wielomian
{
	float *wsp;//tablica wspolczynnikow
	int D;//dlugosc
} W;

W *tabL;

W mult(W x, W y)
{
	int i,j;
	
	W ret;
	ret.D=x.D+y.D;
	ret.wsp=(float*)malloc( (ret.D+1)*sizeof(float));
	
	for (i=0;i<=ret.D;i++)//zerowanie wspolczynnikow wielomianu
	{
		ret.wsp[i]=0;
	}
	for (i=0;i<=x.D;i++)
	{
		for(j=0;j<=y.D;j++)
		{
			ret.wsp[i+j]=ret.wsp[i+j]+x.wsp[i]*y.wsp[j];
		}
	}
	
	return ret;
}

int main()
{
	int i;
	int j;
	float w;

	
	//Wprowadzanie funkcji
	tabwspft[0]=0;
	tabwspft[1]=0;
	tabwspft[2]=0;
	tabwspft[3]=0;
	
	printf("Wprowadz funkcje. Jaka bedzie najwyzsza potega?\n");
	scanf("%d", &power);
	tabwspf=(float*)malloc(power*sizeof(float));
	for (i=0;i<=power;i++)
	{
		printf("Podaj wspolczynnik przy potedze %d\n",i);
		scanf("%f", &w);
		tabwspf[i]=w;
	}
	printf("Czy chcesz uzyc trygonometrii? (1-tak/0-nie)\n");
	int ans;
	scanf("%d",&ans);
	if (ans==1)
	{
		printf("Wprowadz wspolczynnik przy sin(x)\n");
		scanf("%f", &w);
		tabwspft[0]=w;
		
		printf("Wprowadz wspolczynnik przy cos(x)\n");
		scanf("%f", &w);
		tabwspft[1]=w;
		
		printf("Wprowadz wspolczynnik przy tg(x)\n");
		scanf("%f", &w);
		tabwspft[2]=w;
		
		printf("Wprowadz wspolczynnik przy ctg(x)\n");
		scanf("%f", &w);
		tabwspft[3]=w;
	}
		
	//END
	
	//Wprowadzanie granic i ilosci punktow
	printf("Wprowadz dolna granice przedzialu\n");
	scanf("%f", &w);
	borderA=w;
	printf("Wprowadz gorna granice przedzialu\n");
	scanf("%f", &w);
	while(w<=borderA)
	{
		printf("Za mala gorna granica! Wprowadz jeszcze raz\n");
		scanf("%f", &w);
	}
	borderB=w;
	printf("Wprowadz ilosc punktow do interpolacji\n");
	scanf("%d", &quantityP);
	//END
	
	//Ustalanie argumentow i wartosci funkcji
	tabX=(float*)malloc(quantityP*sizeof(float));
	tabY=(float*)malloc(quantityP*sizeof(float));	
	
	tabX[0]=borderA;
	tabX[quantityP-1]=borderB;
	float q=(float)quantityP;
	float interval=(borderB-borderA)/(q-1);
	
	for(i=0;i<quantityP-1;i++)
	{
		tabX[i]=borderA+i*interval;
		if(tabX[i]==0)
		{
			tabX[i]=tabX[i]+0.0000001;
		}
	}
	
	float sumY;
	for(i=0;i<quantityP;i++)//dla kazdego X
	{
		sumY=0;
		for(j=0;j<=power;j++)//pozbieraj w sume wartosc Y
		{
			sumY=sumY+(pow(tabX[i],j))*(tabwspf[j]);
		}
		tabY[i]=sumY+
		tabwspft[0]*(float)sin((double)tabX[i])+
		tabwspft[1]*(float)cos((double)tabX[i])+
		tabwspft[2]*(float)tan((double)tabX[i])+
		tabwspft[3]*( (float)cos((double)tabX[i]) / (float)sin((double)tabX[i]) );
	}
	//END
	
	//TEST
	printf("interwal: %f\n", interval);
	for(i=0;i<quantityP;i++)
	{
		printf("x=%f F(x)=%f\n", tabX[i], tabY[i]);
	}	
	//TEST
	
	//Obliczanie wspolczynnikow Lagrange'a
	tabL=(W*)malloc(quantityP*sizeof(W));
	
	for(i=0;i<quantityP;i++)
	{
		W base;
		base.D=0;
		base.wsp=(float*)malloc( (base.D+1)*sizeof(float));
		base.wsp[0]=1;
		
		j=0;
		while(j<quantityP)
		{
			if(j!=i)
			{
				W subject;
				W outcome;
				subject.D=1;
				subject.wsp=(float*)malloc( (base.D+1)*sizeof(float));
				
				float denom=tabX[i]-tabX[j];
				subject.wsp[1]=1/denom;
				subject.wsp[0]=(-tabX[j])/denom;
				
				outcome=mult(subject, base);
				base = outcome;
			}
			j++;
		}
		tabL[i]=base;
	}
	//END
	
	//Wypisanie wspolczynnikow Lagrange'a i przemnozenie ich przez Y
	for(i=0;i<quantityP;i++)
	{
		printf("L%d=",i);
		for(j=0;j<=tabL[i].D;j++)
		{
			printf("%f*x^%d ",tabL[i].wsp[j], j);
			tabL[i].wsp[j]=tabL[i].wsp[j]*tabY[i];
		}
		printf("\n");
	}
	//END
	
	//Dodanie wspolczynnikow w wielomian interpolacyjny
	W P;
	P.D=quantityP-1;
	P.wsp=(float*)malloc( (P.D+1)*sizeof(float));
	
	for(i=0;i<quantityP;i++)
	{
		float sumP=0;
		for(j=0;j<quantityP;j++)
		{
			sumP=sumP+tabL[j].wsp[i];
		}
		P.wsp[i]=sumP;
	}
	//Wypisanie wielomianu interpolacyjnego do pliku
	FILE *plik;
	plik=fopen("wielomianP.txt","w");
	fprintf(plik,"P(x)=");
	for(i=0;i<P.D;i++)
	{
		fprintf(plik,"(%f)*x^%d+",P.wsp[i],i);
	}
	i=P.D;
	fprintf(plik,"(%f)*x^%d",P.wsp[i],i);
	
	fclose(plik);
	//END	
	getchar();
	return 0;
}

