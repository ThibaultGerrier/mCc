void randomCalculations (int x, int y)
{
    bool xsmaller0;
    bool ysmaller0;
    int div;

    if(x<0){
        xsmaller0=true;
     }else{
        xsmaller0=false;
     }

    if(y<0){
         ysmaller0=true;
      }else{
        ysmaller0=false;
      }

      if(ysmaller0 && xsmaller0){
        print("both values smaller 0");
      }else{
        if(!ysmaller0 && !xsmaller0){
           print("no value smaller than 0");
        }else{
            if(ysmaller0 || xsmaller0){
                print("one value smaller than 0");
            }
        }
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

    string s2;
    s2="Please enter a second number : ";
    print(s2);
    int n2;
    n2=read_int();
    print_nl();

    randomCalculations(n,n2);
}

