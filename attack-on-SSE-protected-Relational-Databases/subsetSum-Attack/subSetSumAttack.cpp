

#include <cstdlib>


#include <cstdio>

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <ctime>



//#define number_of_records 32561
//#define maxNoQueries 498
#define maxNoAttributes 12
#define MaxNoAttempts 2

#define MinNoAttempts 1

#define MAX 33000




using namespace std;

int number_of_records = 0;
int maxNoQueries = 5000;
const int Limit = 5000;

int target_cardinality = 2;

int Ct[Limit+1][Limit+1] = {{0}};
int no_solutions = 0;

bool** dp;


class Attribute
{
public:
    string name;
    int cardinality;
    int attempts_to_resolve;
};

class Query
{
public:
    int query_no;
    int result_size;
    vector<Query> *queryList;
};

class CompResultSize
{
public:
    inline bool operator() (const Query& q1, const Query& q2)
    {
        return (q1.result_size < q2.result_size);
    }
};

bool operator< (Query qi,Query qj) { int xi,xj; if(qi.queryList == NULL) xi = 0; else xi = qi.queryList->size(); if(qj.queryList==NULL) xj=0; else xj = qj.queryList->size(); 

if(xi == 0 || xj == 0 || xi==xj)
     return qi.result_size < qj.result_size; 
return xi<xj;
}

std::ostream& operator<<(std::ostream& out, const Query& q){ if(q.queryList == NULL) out<<"(0,"<<q.query_no<<","<<q.result_size<<")"; else out<<"("<<q.queryList->size()<<","<<q.query_no<<","<<q.result_size<<")";}

bool operator== (Query qi,Query qj) { return (qi.query_no==qj.query_no) && (qi.result_size == qj.result_size);}

bool isQueryRemoved(Query q){return (q.query_no == -1);}
bool isEqual(Query qi,Query qj) { return (qi.query_no==qj.query_no && qi.result_size == qj.result_size);}

bool isQueryExist(const Query& q, const vector<Query>& vq)
{
    
    for(int i = 0;i < vq.size();i++)
        if(vq[i] == q)
            return true;
    return false;
}




class CompQueryNo
{
  public:
    inline bool operator() (const Query& q1, const Query& q2)
    {
        //if(q1.result_size == q2.result_size)    
           return (q1.query_no < q2.query_no);
        
       //return (q1.result_size < q2.result_size);
        
    }
};


vector<Query> correctSolutions[MAX];

vector<Attribute> targetAttributes;


void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


//------------------------------------------------------------------------------

vector<Query> intersection(const vector<Query>& v1,const vector<Query>& v2)
{
    vector<Query> v1v2;
    
    
    for(int i = 0;i < v1.size();i++)
        for(int j = 0;j < v2.size();j++)
            if(v1[i] == v2[j])
                v1v2.push_back(v1[i]);
        
        
    return v1v2;
    
}

//-------------------------------------------------------------------------------

bool testZeroCo_Occurrence(const vector<Query>& v)
{
        for(int i = 0;i < v.size();i++)
        {
            for(int j = i+1;j < v.size();j++)
            {
                if(Ct[v[i].query_no][v[j].query_no] != 0)
                {
                    return false;
                }
            }
        }
        
        
        return true;
}


//-------------------------------------------------------------------------------

bool isValidCardinality(vector<Attribute> a, int card)
{
    for(int i = 0;i < a.size();i++)
        if(a[i].cardinality == card)
            return true;
    return false;
}

//----------------------------------------------------------------------------------
int sumResultSizes(vector<Query>& v, int begin, int end)
{
    int sum = 0;
    
    for(int i = begin; i <= end;i++)
        sum += v[i].result_size;
    
    return sum;
}
//------------------------------------------------------------------------------------

vector<Query> subQueryVector(vector<Query> v, int begin, int end)
{
    
    vector<Query> result;
    
    
    for(int i = begin; i <= end;i++)
        result.push_back(v[i]);
    
    return result;
    
}

//-----------------------------------------------------------------------------------
void saveCorrectSolution(vector<Query>& v) {

       
    if(isValidCardinality(targetAttributes,v.size()))//if(v.size() >= target_cardinality)
    {
        
        //if(testZeroCo_Occurrence(v)==true)
        //{
          
          no_solutions++;
          
          correctSolutions[no_solutions] = v; 
          
        //}
        
        
    }
}


//--------------------------------------------------------------------------------------

void display(const vector<Query>& v)
{ 
      for (int i = 0; i < v.size(); ++i)
             cout<<v[i].query_no<<":"<<v[i].result_size<<";";
      cout<<endl;
          
}
//-----------------------------------------------------------------------------------------

void output(const vector<Query>& queryList, int i, int sum, vector<Query>& p) {
    
    if (i == 0 && sum != 0 && dp[0][sum]) {
        p.push_back(queryList[i]);
          
        if(testZeroCo_Occurrence(p)==true){
            
           saveCorrectSolution(p);
        }
        return;
    }
    if (i == 0 && sum == 0) {
        saveCorrectSolution(p);
        return;
    }
    
    if (sum == 0) {
        saveCorrectSolution(p);
        return;
    }
    
    
    if (dp[i - 1][sum]) {
        vector<Query> b = p;
        output(queryList, i - 1, sum, b);
    }
    if (sum >= queryList[i].result_size && dp[i - 1][sum - queryList[i].result_size]) {
        p.push_back(queryList[i]);
        
        if(testZeroCo_Occurrence(p)==true)
            output(queryList, i - 1, sum - queryList[i].result_size, p);
    }
}


//---------------------------------------------------------------------------------------------------
bool subsetSum(vector<Query>& queryList, int sum) {
  
    if (queryList.size() == 0 || sum < 0) 
        return false;
    if(dp != NULL)
    {
        delete dp;
        dp = NULL;
    }
    dp = new bool*[queryList.size()];
    for (int i = 0; i < queryList.size(); ++i) {
        dp[i] = new bool[sum + 1];
        dp[i][0] = true;
    }
    for (int i = 1; i < sum + 1; ++i)
        dp[0][i] = (queryList[0].result_size == i) ? true : false;
    for (int i = 1; i < queryList.size(); ++i)
        for (int j = 0; j < sum + 1; ++j)
            dp[i][j] = (queryList[i].result_size <= j) ? dp[i - 1][j] || dp[i - 1][j - queryList[i].result_size] : dp[i - 1][j];
    
    if (!dp[queryList.size() - 1][sum]) {
        return false;
    }
    
    return true;
    
    
}

//-------------------------------------------------------------

void ksubset(vector<Query> &q, int size, int k, int index, vector<Query> &p){
    if(k==0){
        
          //saveCorrectSolution(p);//print(l);
        //testZeroCo_Occurrence(v)==true
        int sum = 0;
        for(int i = 0;i < p.size();i++)
            sum+=p[i].result_size;
        if(sum == number_of_records && testZeroCo_Occurrence(p)==true)
            saveCorrectSolution(p);
            
        return;
    }
    for(int i=index; i<size;i++){
        p.push_back(q[i]);
        
        ksubset(q,size,k-1,i+1,p);
   
        p.pop_back();
    }

}  


//-------------------------------------------------------------



/*
 * 
 */
int main(int argc, char** argv) 
{ 


    

    int x, y, f; //read the three integers, queries number x and y and their joint frequency value f 
    char c1, c2; //read two commas


    
    if(argc != 5)
    {

       std::cout<<"./attack observedMatrixFile no_records maxNoQueries meta_data_info\n";
       exit(1);
    }

    
    std::ifstream infile(argv[1]);//"adult498l/observed10-498-498.txt");//495_1.txt");
    
    number_of_records =  atoi(argv[2]);

    maxNoQueries = atoi(argv[3]);
    
    
    std::ifstream metaDataInfo(argv[4]);
    
    
    for(int i = 0;i < maxNoQueries+1;i++)
        for(int j = 0;j < maxNoQueries+1;j++)
            Ct[i][j] = 0;
           
    
    while (infile >> x >> c1 >> y >> c2 >> f)
    {

      Ct[x][y] = f;
      Ct[y][x] = f;
      
      //cout<<"Ct["<<x<<"]["<<y<<"] = "<<Ct[x][y]<<";"<<endl;
    }

   
    
    
    
    vector<Query> recoveredQueries;
    
    //for each query q_i, attach all other queries q_j satisfying Ct[q_i][q_j] = 0
    
    
    //vector<int> queriesSize;
    vector<Query> queries;
    
    //queriesSize.push_back(0);//query number 0 means non-existant query
    
    Query dummy;
    dummy.query_no = 0;
    dummy.result_size = 0;
    dummy.queryList = NULL;
    queries.push_back(dummy);
    
    for(int i = 1;i < maxNoQueries+1;i++)
    {
        
        
        Query qi;
        
        qi.query_no = i;
        qi.result_size = Ct[i][i];
        
        qi.queryList = new vector<Query>();
        
        qi.queryList->push_back(qi);
        
        for(int j = 1;j < maxNoQueries+1;j++)
        {
            
            if(Ct[i][j] == 0)
            {
               Query qj;
               qj.query_no = j;
               qj.result_size = Ct[j][j];
               qj.queryList = NULL;
               qi.queryList->push_back(qj);  
            }
            
            
            
            
            
            
            
            //check subset sum here
            
            
            
            
            
            
        
        }
            
        //queriesSize.push_back(qi.queryList->size());
        
        queries.push_back(qi);
        
        
    }
    
    
    
    //sort the queries according to their result sizes
    
 
    
    std::sort (queries.begin(), queries.end());
    
    
    // print out content:
    std::cout << "Queries vector contains:";
    for (int i = 0;i < maxNoQueries+1;i++)
      std::cout << ' ' << queries[i];
    std::cout << '\n';
    
    
    
    
     //add each attribute in the file
    
    
    for( std::string line; getline( metaDataInfo, line ); )
    {
        //...for each line in inpu
        vector<string> tokens; // Create vector to hold our words

    
        Tokenize(line, tokens, ":");
        Attribute attributeObj;
        
        attributeObj.name = tokens[0];
        attributeObj.cardinality = atoi(tokens[1].c_str());
        
        targetAttributes.push_back(attributeObj);
        
        cout<<"name = "<<attributeObj.name<<", card = "<<attributeObj.cardinality<<endl;
    }
    
    metaDataInfo.close();
    
    int total_no_of_solutions = 0;
    
    
    
    
    
    
    
    //queries is L in Algorithm 1
    cout<<"Target attributes listed below."<<endl;
    
    for(int i = 0;i < targetAttributes.size();i++)
        cout<<i+1<<":"<<targetAttributes[i].name<<","<<targetAttributes[i].cardinality<<endl;
    
    
    int ctr = 0;
    
    
    clock_t begin = clock();
    
    
    
    
    
    ctr = 1;
      
      
          
          
    while((ctr < maxNoQueries) && (targetAttributes.size() >= 1))
    {
            
            //if(targetAttributes.size() == 0)
               // break;
            
            if(queries[ctr].queryList != NULL)
            {  
              if(!isQueryExist(queries[ctr].queryList->at(0),recoveredQueries))
              {    
                  
                   vector<Query> currentQueryList;
                     
                   for(int j = 0;j < queries[ctr].queryList->size();j++)
                          if(!isQueryExist(queries[ctr].queryList->at(j),recoveredQueries))
                                  currentQueryList.push_back(queries[ctr].queryList->at(j));
                          
                   cout<<"current ctr = "<<ctr<<", size = "<<queries[ctr].queryList->size()<<", new current size = "<<currentQueryList.size()<<endl;
          
                   for(int s = 0;s < currentQueryList.size();s++)
                       cout<<currentQueryList[s]<<"\t";//currentQueryListResultSizes.push_back(currentQueryList[s].result_size);
                   cout<<endl;
          
          
                  cout<<"Resolving queries with cardinalties equal to ";
                  
                  for(int i = 0;i < targetAttributes.size();i++)
                      cout<<targetAttributes[i].cardinality<<", ";
                  cout<<endl;
          
                  int target_sum = number_of_records-currentQueryList[0].result_size;
                  
                  vector<Query> sortCurrent = currentQueryList;
                  
    
                  std::sort (sortCurrent.begin(), sortCurrent.end());
                  
                  vector<Query> mergeQuery = sortCurrent;
                          
                  
                  
                  if(subsetSum(mergeQuery , target_sum ))
                  {
             
              
                     cout<<"Subset sum exist..........set size = "<<mergeQuery.size()<<endl;
             
                     vector<Query> p;
             
    
                     p.push_back(currentQueryList[0]); //let each solution include the first element in queryList since sum = number_of_records-queryList[0].resultsize
             
            
                     
    
                     //std::sort (currentQueryList.begin(), currentQueryList.end());
    
                            
                    for(int s = 0;s < mergeQuery.size();s++)
                       cout<<mergeQuery[s]<<"\t";//currentQueryListResultSizes.push_back(currentQueryList[s].result_size);
                    cout<<endl;
          
    
                     //output(currentQueryList, currentQueryList.size() - 1, target_sum, p);
             
                     //output(sortCurrent, sortCurrent.size() - 1, target_sum, p);
             
                     output(mergeQuery, mergeQuery.size() - 1, target_sum, p);
             
              
                     cout<<"current no of solutions = "<<no_solutions<<endl;
    
                     
      
                     if(no_solutions == 1)
                     {
             
                        vector<Query> sol = correctSolutions[1];
          
               
                        cout<<"One solution exists at ctr = "<<ctr<<" with card. = "<<sol.size()<<endl; 
               
               
                        for(int i = 0; i < targetAttributes.size();i++)
                          if(targetAttributes[i].cardinality == sol.size())
                          {
                             targetAttributes.erase(targetAttributes.begin() + i);
                             break;
                          }
               
                        //sort sol
               
               
                        std::sort (sol.begin(), sol.end(),CompQueryNo());
               
               
                        for(int i = 0;i < sol.size();i++)
                            recoveredQueries.push_back(sol[i]);
                        
                        
                        
                        //std::sort (sol.begin(), sol.end());
               
                        display(sol);
             
          
               
                        total_no_of_solutions += no_solutions;
                        no_solutions = 0;
                        //break;
          
                
                    }
                    else if(no_solutions > 1)
                    {
       
                      if(no_solutions >  maxNoAttributes)
                      {
                         cout<<no_solutions <<" possible solutions exist at ctr = "<<ctr<<" with card. = "<<correctSolutions[1].size()<<endl;
                         
                         for(int i = 0; i < targetAttributes.size();i++)
                          if(targetAttributes[i].cardinality == correctSolutions[1].size())
                          {
                             targetAttributes.erase(targetAttributes.begin() + i);
                             break;
                          }  
                         
                      }
                      
                      //cout<<"So our attack cannot decide which is the correct one and we leave it to be decided when the attacker gains background knowledge. We increment ctr and try next query list in order to see other possible solutions.\n";
          
                
                    
                      vector<Query> sol;
          
                      for(int i = 1; i <= no_solutions;i++)
                      {
                         sol = correctSolutions[i];
          
                  
                         for(int j = 0;j < sol.size();j++)
                         {
                      
                 
                            if(!isQueryExist(sol[j],recoveredQueries))                    
                               recoveredQueries.push_back(sol[j]);
                         }
                  
                         if(no_solutions <=  maxNoAttributes)
                         {
                            cout<<"Solution exists at ctr = "<<ctr<<" with card. = "<<sol.size()<<endl; 
                            display(sol);
                         }
                
                      }
          
          
                      total_no_of_solutions += no_solutions;
          
                      no_solutions = 0;
                      //break;
                   }
            
           
           
                
      
          
                }        
                else 
                       cout<<"There are no subsets with sum "<<number_of_records<<" in the current list whose head is "<<currentQueryList[0]<<". Try next list."<<endl;
             
                   
              }
              else
              {
                  cout<<"Already resolved ... ctr = "<<ctr<<". Look for a new ctr"<<endl;
              }
            }
            else 
                cout<<"NULL at ctr = "<<ctr<<endl;
                
            
           
            ctr++;
            
            
            cout<<"ctr = "<<ctr<<", size = "<<targetAttributes.size()<<endl;
            
    }  
          
          
          
          
 
             
             
      
      
      
    
    
   
    
   
    
   //Executed when Hours-per-week and AGE are not considered 
   clock_t end = clock();
   double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
   
   
  
  
   cout<<"Total queries recovered = "<<recoveredQueries.size()<<endl;
    
   cout<<"Total time in mins = "<< elapsed_secs/60 <<endl;
    
  
   if(dp != NULL)
   {
        delete dp;
        dp = NULL;
   }
    
    
    
    
   return 0;

}


