int add (int x, int y, bool subtract) /*subtract for add/sub switch*/
{
    if(x>-1 && y>-1){
        if(!subtract){
            return x+y;
        }else{
            return x-y;
        }
    }else{
        return -1;
    }
}


void main()
{
    string s;
    s="Please enter a number : ";
	print(s);
	int n;
	n=read_int();
    print_nl();


	int[10] a;
	int num;
	num=0;
	while (num<n){
	    a[num]=num;
	    num=num+1;
	}
	int num2;
    num=0;
    bool sub;
    sub=true;
	while (num2<n){
        int result;
        result=add(a[num2],4,sub);
        print_int(result);
        print_nl();
        num=num-1;
    	}


}
