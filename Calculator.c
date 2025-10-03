#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 100

int Operator(char c){
    if(c=='+'||c=='-'||c=='*'||c=='/')
        return 1;
    else
        return 0;
}

int solve(char *exp, int *err){
    int nums[SIZE], numcount=0;
    char operatorlist[SIZE]; 
    int opcount=0;
    int i=0, j=0, k;
    char last = '\0';   

    while(exp[i] != '\0'){
       
        if(exp[i] == ' '){
            i++;
            continue;
        }

        
        if(isdigit(exp[i])){  
            int x = 0;
            while(isdigit(exp[i])){  
                x = x*10 + (exp[i]-'0');
                i++;
            }
            nums[numcount++] = x;
            last = 'n'; 
        }

        
        else if(Operator(exp[i])){
            int look = i+1;
            while(exp[look] == ' ') look++; 
            if(exp[look] == '\0'){  
                *err = 2;
                return 0;
            }
            if(!isdigit(exp[look])){ 
                *err = 2;
                return 0;
            }

            operatorlist[opcount++] = exp[i];
            last = exp[i];
            i++;
        }

        // Invalid character
        else{
            *err = 2;
            return 0;
        }
    }

    if(last != 'n'){  
        *err = 2;
        return 0;
    }

    // Now handle * and /
    while(j < opcount){
        if(operatorlist[j] == '*' || operatorlist[j] == '/'){
            if(operatorlist[j] == '/'){
                if(nums[j+1] == 0){
                    *err = 1; 
                    return 0;
                }
                nums[j] = nums[j] / nums[j+1];
            }
            else{
                nums[j] = nums[j] * nums[j+1];
            }

            for(k=j+1;k<numcount-1;k++) nums[k] = nums[k+1];
            for(k=j;k<opcount-1;k++) operatorlist[k] = operatorlist[k+1];
            numcount--;   
            opcount--;    
        }
        else j++;
    }

    // Handle + and -
    int ans = nums[0];
    for(j=0;j<opcount;j++){
        if(operatorlist[j] == '+') 
            ans += nums[j+1];
        else 
            ans -= nums[j+1];
    }
    return ans;
}

int main(){
    char exp[SIZE];
    printf("Enter expression: ");
    fgets(exp, SIZE, stdin);
    exp[strcspn(exp, "\n")] = '\0'; 

    int err = 0;
    int res = solve(exp, &err);

    if(err == 1){
        printf("Error: Division by zero.\n");
    }
    else if(err == 2){
        printf("Error: Invalid expression.\n");
    }
    else{
        printf("%d\n", res);
    }
    return 0;
}

