static int fileHandle;
int i,j,length=0,status,token,numLine,flag,count,randomArray[100]={0};
char *fileName="quesions.txt";
char lineBuffer[200],values[10240000];
//int MemLocation, length;



int ReadNumOfLine(int fileHandle)
{
i=0;
do
{
ReadLine (fileHandle, values, 20);
length++;
i++;
}while(i!=-2);

return length;
}


int GenerateOddUniqueRandom()
{

			fileHandle = OpenFile (fileName, VAL_READ_ONLY, VAL_OPEN_AS_IS, VAL_BINARY);
			
			for (i=0;status!=-2;i++)
			{
			for(j=0;j<strlen(lineBuffer);j++)
			lineBuffer[j]=' ';
			
			RemoveSurroundingWhiteSpace(lineBuffer);
			status=ReadLine (fileHandle, lineBuffer, 100);
			numLine++;
			}
			
			do
			{
				flag=1;
				SetRandomSeed(0);
				token=Random(1,numLine/2);
				count++;
					for(i=0;i<100;i++)
					{
						if(randomArray[i]==token) flag=0;
					}
			}while(flag==0 && count<pow(numLine,5) && token%2==0);
			
			if (flag==1)
			{
				for(i=0;randomArray[i]!=0;i++) continue;
			randomArray[i]=token;
			}

			if (flag==0)
			{
			MessagePopup ("Warning!", "We don't have any more questions for you!");
			}
			

			CloseFile (fileHandle);
 return token;
}


void ClearArray(char *MemLocation,int length)
{
for (i=0;i<length;i++)
*MemLocation=' ';
}

