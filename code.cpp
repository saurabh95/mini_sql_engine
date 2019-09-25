#include <bits/stdc++.h>
using namespace std;


typedef map<string, map<string, vector<int> > > dbms;
typedef map<string, map<int, string> > db_ref;

bool error;

struct full_db{
    dbms data;
    db_ref meta;
};



struct full_query{
    vector < string > columns;
    vector < string > tables;
    vector < string > conditions;
    vector < string > operators;
    vector < string > functions;
};

struct full_miner{
    int n;          //number of tables
    dbms data;
    map < string , int > m;     //map of tablename.columnname to index
    vector < string > table;    // list of tables
    vector < int > size;
    vector < vector < string > > column;
    vector < int > pointer;
    vector < int > maybe;
};


vector< vector < int > > join_table;
set< string > to_remove;

bool isnumber(string s)
{
    if(s[0]=='+' || s[0]=='-' || isdigit(s[0]))
    {
        for(int i=1;i<s.size();i++)
            if(!isdigit(s[i]))
                return false;
    }
    else
        return false;
    return true;
}

bool func_handler(string func,struct full_db * db,struct full_query * query)
{
    if(func == "max" || func == "min" || func == "avg" || func == "sum")
    {
        string col = query->columns[0];
        size_t pos = col.find(".");
        col = col.substr(pos+1);
        string table_name = query->tables[0];
        vector<int> final = db->data[table_name][col];
        int gmax = INT_MIN, gmin = INT_MAX;
        long long int gsum = 0;
        double gavg = 0.0;
        for(int i=0;i<final.size();i++)
        {
            gmax = max(gmax, final[i]);
            gmin = min(gmin, final[i]);
            gavg += final[i];
        }
        gsum = gavg;
        gavg = 1.0*gavg/(1.0*final.size());
        cout << func << "(" << query->columns[0] << ")" << endl;
        if(func=="max")
            cout << gmax << endl;
        else if(func=="min")
            cout << gmin << endl;
        else if(func=="avg")
        {
            cout.setf(ios::fixed);
            cout<<setprecision(4)<<gavg<<endl;
        }
        else if(func=="sum")
            cout << gsum << endl;
        return true;
    }
    return false;
}

void select_from_db(struct full_miner * miner,struct full_query * query)
{
    int flag = 0;
    //Printing columns heading
    for(int i=0;i<query->columns.size();i++)
    {
      //  cout << "THIS " << query->functions[i] << endl;
        if(i!=0 && flag==1)
        {
            flag = 0;
            cout << ",";
        }
        if(to_remove.find(query->columns[i])==to_remove.end())
        {
            flag = 1;
           // cout << i << " IIIIIII\n";
         //   if(query->functions[i]=="distinct")
         //       cout << "distinct(";
            cout << query->columns[i];
         //   if(query->functions[i]=="distinct")
           //     cout << ")";
        }
    }
    cout << endl;

    //Printing data
    for(int i=0;i<join_table.size();i++)
    {
        for(int j=0;j<query->columns.size();j++)
        {
            if(j!=0 && flag==1)
            {
                flag = 0;
                cout << ",";
            }
            if(to_remove.find(query->columns[j])==to_remove.end())
            {
                cout << join_table[i][miner->m[query->columns[j]]];
                flag = 1;
            }
        }
        cout << endl;
    }

}

bool check_where(struct full_miner *miner,string op,struct full_query * query)
{
    vector <int> candidate(miner->maybe);
    string token,comp;
    stringstream ss;
    ss << op;
    vector<string> check;
    while(ss>>token)
        check.push_back(token);
    comp = check[1];
    if(comp=="=")
    {
        if(isnumber(check[0]))
        {
            if(atoi(check[0].c_str())==candidate[miner->m[check[2]]])
                return true;
        }
        else if(isnumber(check[2]))
        {
            if(candidate[miner->m[check[0]]]==atoi(check[2].c_str()))
                return true;
        }
        else if(candidate[miner->m[check[0]]]==candidate[miner->m[check[2]]])
        {
            //removing duplicate column
            to_remove.insert(check[2]);
            return true;
        }
    }
    else if(comp==">")
    {
        if(isnumber(check[0]))
        {
            if(atoi(check[0].c_str()) > candidate[miner->m[check[2]]])
                return true;
        }
        else if(isnumber(check[2]))
        {
            if(candidate[miner->m[check[0]]] > atoi(check[2].c_str()))
                return true;
        }
        else if(candidate[miner->m[check[0]]] > candidate[miner->m[check[2]]])
        {
            //removing duplicate column
          // to_remove.insert(check[2]);
            return true;
        }
    }
    if(comp==">=")
    {
        if(isnumber(check[0]))
        {
            if(atoi(check[0].c_str()) >= candidate[miner->m[check[2]]])
                return true;
        }
        else if(isnumber(check[2]))
        {
            if(candidate[miner->m[check[0]]] >= atoi(check[2].c_str()))
                return true;
        }
        else if(candidate[miner->m[check[0]]] >= candidate[miner->m[check[2]]])
        {
            //removing duplicate column
          // to_remove.insert(check[2]);
            return true;
        }
    }
    if(comp=="<")
    {
        if(isnumber(check[0]))
        {
            if(atoi(check[0].c_str()) < candidate[miner->m[check[2]]])
                return true;
        }
        else if(isnumber(check[2]))
        {
            if(candidate[miner->m[check[0]]] < atoi(check[2].c_str()))
                return true;
        }
        else if(candidate[miner->m[check[0]]] < candidate[miner->m[check[2]]])
        {
            //removing duplicate column
         //  to_remove.insert(check[2]);
            return true;
        }
    }
    if(comp=="<=")
    {
        if(isnumber(check[0]))
        {
            if(atoi(check[0].c_str()) <= candidate[miner->m[check[2]]])
                return true;
        }
        else if(isnumber(check[2]))
        {
            if(candidate[miner->m[check[0]]] <= atoi(check[2].c_str()))
                return true;
        }
        else if(candidate[miner->m[check[0]]] <= candidate[miner->m[check[2]]])
        {
            //removing duplicate column
     //      to_remove.insert(check[2]);
            return true;
        }
    }
    if(comp=="!=")
    {
        if(isnumber(check[0]))
        {
            if(atoi(check[0].c_str())!=candidate[miner->m[check[2]]])
                return true;
        }
        else if(isnumber(check[2]))
        {
            if(candidate[miner->m[check[0]]]!=atoi(check[2].c_str()))
                return true;
        }
        else if(candidate[miner->m[check[0]]]!=candidate[miner->m[check[2]]])
        {
            //removing duplicate column
          /// to_remove.insert(check[2]);
            return true;
        }
    }
    return false;
}


bool accept_on_where(struct full_miner* miner, struct full_query* query)
{
    if(query->conditions.size()==0)
        return true;
    bool accept = true;
    vector<bool> accepts;
    for(int i=0;i<query->conditions.size();i++)
        accepts.push_back(check_where(miner, query->conditions[i], query));
    if(query->operators.size()==0)
        return accepts[0];
    accept = accepts[0];
    for(int i=0;i<query->operators.size();i++)
        if(query->operators[i]=="and")
            accept = accept & accepts[i+1];
        else if(query->operators[i]=="or")
            accept = accept | accepts[i+1];
    return accept;
}


void row_checker(struct full_miner * miner,struct full_db * db, struct full_query* query)
{
    vector <int> p;
    p = miner->pointer;
    for(int i=0;i<p.size();i++)
    {
        string table_name = miner->table[i];
        for(int j=0;j<miner->column[i].size();j++)
        {
            string tmp=table_name+"."+miner->column[i][j];
            miner->maybe[miner->m[tmp]]=db->data[table_name][miner->column[i][j]][miner->pointer[i]];
        }
    }
    if(accept_on_where(miner,query))
    {
        join_table.push_back(miner->maybe);
    }
    int le=miner->maybe.size();
//    for(int i=0;i<le;i++)
  //      cout<<miner->maybe[i]<<" ";
  //  cout<<endl;
  //  cout << endl;
}


struct full_miner * resolve_carry(struct full_miner * miner,struct full_db * db,struct full_query* query)
{
    int last = miner->n -1;
    int first = 0;
    while(miner->pointer[first]>=miner->size[first])
    {
        if(last==first)
            break;
        miner->pointer[first+1]+=1;
        miner->pointer[first] = 0;
        first++;
    }
    if(miner->pointer[last] < miner->size[last])
        row_checker(miner, db, query);
    return miner;
}

void mine(struct full_miner * miner,struct full_db * db, struct full_query* query)
{
    int last = miner->n - 1;
    while(1)
    {
        miner = resolve_carry(miner, db, query);
        miner->pointer[0]+=1;
        if(miner->pointer[last]>=miner->size[last])
            break;
    }
}



struct full_miner * prepare_mining(struct full_query * query , struct full_db * db)
{
    int count = 0;
    map <string, int > maps;
    vector<string> tables;
    vector <int> sizes;
    vector <vector <string> > columns;
    for(int j=0;j<query->tables.size();j++)
    {
        string tmp = query->tables[j];
        vector <string> tmpv;
        map < string , vector < int > > ::iterator it;
        string y = "";
        for(it = db->data[tmp].begin();it!=db->data[tmp].end();it++)
        {
            string x = tmp + "." + it->first;
            tmpv.push_back(it->first);
            maps[x] = count;
            count++;
            y = it->first;
        }
        tables.push_back(tmp);
        columns.push_back(tmpv);
        int l = db->data[tmp][y].size();
        sizes.push_back(l);
    }
    vector < int > pointers;
    vector < int > potential;

    for(int i=0;i<count;i++)
    {
        potential.push_back(0);
    }
    int n_tables=sizes.size();
    for(int i=0;i<n_tables;i++)
    {
        pointers.push_back(0);
    }
    full_miner * full = new full_miner;
    full->data=db->data;
    full->table=tables;
    full->m=maps;
    full->column=columns;
    full->size=sizes;
    full->pointer=pointers;
    full->maybe=potential;
    full->n=n_tables;
    return full;
}








struct full_query* fill_functions(struct full_query* query)
{
    for(int i=0;i<query->columns.size();i++)
    {
        string col = query->columns[i];
        size_t pos;
        if(col.find("distinct")!=string::npos)
        {
            pos = col.find("distinct");
            //        cout << "DISTINCT\n";
            query->functions.push_back("distinct");
            int l = col.size();
            //      cout << "LENGTH: " << l << endl;
            query->columns[i] =  col.substr(pos+9, l-10);
            continue;
        }
        else if(col.find("sum")!=string::npos)
        {
            pos = col.find("sum");
            query->functions.push_back("sum");
            int l = col.size();
            query->columns[i] = col.substr(pos+4, l-5);
            continue;
        }
        else if(col.find("avg")!=string::npos)
        {
            pos = col.find("avg");
            query->functions.push_back("avg");
            int l = col.size();
            query->columns[i] = col.substr(pos+4, l-5);
            continue;
        }
        else if(col.find("max")!=string::npos)
        {
            pos = col.find("max");
            query->functions.push_back("max");
            int l = col.size();
            query->columns[i] = col.substr(pos+4, l-5);
            continue;
        }
        else if(col.find("min")!=string::npos)
        {
            pos = col.find("min");
            query->functions.push_back("min");
            int l = col.size();
            query->columns[i] = col.substr(pos+4, l-5);
            continue;
        }
        else
            query->functions.push_back("NULL");
      //  cout << query->functions[i] << endl;
    }
    return query;
}

struct full_query* parse_query(string input)
{
    for(int i=0;i<input.size();i++)
        if(input[i]==',')
            input[i] = ' ';
    stringstream ss;
    ss << input;
    string token;
    string temp;
    full_query* query = new full_query;
    int flag1 = -1, flag2=0;
    while(ss >> token)
    {
        //   transform(token.begin(), token.end(), token.begin(), ::tolower);
        if(flag1==-1 && (token.compare("select")==0 || token.compare("SELECT")==0))
        {
            flag1 = 0;
            continue;
        }
        else if(flag1==0 && (token.compare("from")==0 || token.compare("FROM")==0))
        {
            flag1 = 1;
            continue;
        }
        else if(flag1==1 && (token.compare("where")==0 || token.compare("WHERE")==0))
        {
            flag1 = 2;
          //  cout << "GONE\n\n";
            continue;
        }
        if(flag1 == 0)
            query->columns.push_back(token);
        else if(flag1==1)
            query->tables.push_back(token);
        else if(flag1==2)
        {
            if(token.compare("and")==0 || token.compare("or")==0 || token.compare("OR")==0 || token.compare("AND")==0)
            {
                //        cout << "GONE\n";
                query->operators.push_back(token);
                if(temp.compare("")!=0)
                {
                    string leftop="", rightop="", op="";
                    for(int i=0;i<temp.size();i++)
                    {
                        if(temp[i]=='<' || temp[i]=='>' || temp[i]=='=' || temp[i]=='!')
                            flag2 = 1;
                        else if(flag2==1 and (temp[i]!='<' and temp[i]!='>'  and temp[i]!='!'))
                            flag2 = 2;
                        if(flag2==0 && temp[i]!=' ')
                            leftop+=temp[i];
                        if(flag2==1 && temp[i]!=' ')
                            op+=temp[i];
                        if(flag2==2 && temp[i]!=' ')
                            rightop+=temp[i];
                    }
                    string q = leftop + " " + op + " " + rightop;
                    //          cout << "QUERY: " << q << endl;
                    query->conditions.push_back(q);
                }
                temp = "";
                continue;
            }
            temp+=token;
            temp+=" ";
        }
    }
    flag2 = 0;
    if(temp.compare("")!=0)
    {
        string leftop="", rightop="", op="";
        //  cout << "dadasdas" << temp << endl;
        for(int i=0;i<temp.size();i++)
        {
            if(temp[i]=='<' or temp[i]=='>' or temp[i]=='=' or temp[i]=='!')
                flag2 = 1;
            if(flag2==1 and (temp[i]!='<' and temp[i]!='>' and temp[i]!='=' and temp[i]!='!'))
                flag2 = 2;
            if(flag2==0 && temp[i]!=' ')
                leftop+=temp[i];
            if(flag2==1 && temp[i]!=' ')
                op+=temp[i];
            if(flag2==2 && temp[i]!=' ')
                rightop+=temp[i];
        }
        //  cout << "Right: " << leftop << endl;
        string q = leftop + " " + op + " " + rightop;
        query->conditions.push_back(q);
    }
    if(query->columns.size() == 0 || query->tables.size()==0)
        error = true;
    return query;
}


struct full_db* populate_db(full_db* db)
{
    map<string, map<string, vector<int> > >::iterator it1;
    map<string, vector<int> > cols;
    map<string, vector<int> >::iterator it2;
    for(it1 = db->data.begin();it1!=db->data.end();it1++)
    {
        string filename = it1->first;
        filename.append(".csv");
        //        cout << filename << endl;
        ifstream datafile(filename.c_str());
        string token;
        while(datafile>>token)
        {
            // cout << token << endl;
            for(int l = 0;l<token.size();l++)
                if(token[l] == ',')
                    token[l] = ' ';
            // cout << token << endl;
            stringstream ss;
            ss << token;
            string temp;
            int col_number = 1;
            while(ss>>temp)
            {
                string col_name = db->meta[it1->first][col_number];
                int val = stoi(temp);
                db->data[it1->first][col_name].push_back(val);
                //   cout << temp << " ";
                col_number++;
            }
            //  cout << endl;
        }
        datafile.close();
    }
    return db;

}

struct full_db* getMetaData()
{
    ifstream meta("metadata.txt");

    string token, table_name;
    dbms data;
    db_ref temp_meta;
    int col_number;
    int flag=1;
    while(meta>>token)
    {
        if(flag==0)
        {
            if(token[0]=='<')
                flag = 1;
            else
            {
                vector<int> temp_col;
                data[table_name][token] = temp_col;
                temp_meta[table_name][++col_number] = token;
            }
        }
        else
        {
            if(token[0]=='<')
                flag = 0;
            meta >> table_name;
            map <string, vector<int> > col;
            data[table_name] = col;
            col_number = 0;
        }
    }
    meta.close();
    full_db* new_db = new full_db;
    new_db->data = data;
    new_db->meta = temp_meta;
    return new_db;
}

void print_MetaData(full_db * db)
{
    map<string, map<string, vector<int> > >::iterator it1;
    map<string, vector<int> > cols;
    map<string, vector<int> >::iterator it2;
    for(it1 = db->data.begin();it1!=db->data.end();it1++)
    {
       // cout << it1->first << endl;
        cols = it1->second;
       // for(it2 = cols.begin();it2!=cols.end();it2++)
         //   cout << it2->first << " ";
        it2 = cols.begin();
        int size = it2->second.size();
        // cout << "SIZE: " << size << endl;
        for(int i=0;i<size;i++)
        {
      //      for(it2 = cols.begin();it2!=cols.end();it2++)
        //        cout << it2->second[i] << " ";
          //  cout << endl;
        }
      //  cout << endl;
    }
}


struct full_query* verify_wrapper(struct full_db* db, struct full_query* query, string cur_column, int index_string, bool flag)
{
    size_t found_pos;
    found_pos = cur_column.find('.');

    // for query like select table1.A from table1
    if(found_pos!=string::npos)
    {
        if(db->data.find(cur_column.substr(0,found_pos))==db->data.end())
            error = true;
        else if(db->data[cur_column.substr(0,found_pos)].find(cur_column.substr(found_pos+1)) == db->data[cur_column.substr(0,found_pos)].end())
        {
            //          cout << "ERROR: " << cur_column << endl;
            error = true;
        }
    }
    else
    {
        int count = 0;
        string newcolname="", mycomp = "";
        for(int i=0;i<query->tables.size();i++)
        {
            if(db->data[query->tables[i]].find(cur_column) != db->data[query->tables[i]].end())
            {
                //                cout << "GONE\n";

                count++;
                if(flag)
                {
                    newcolname=query->tables[i] + "." + cur_column;
                    query->columns[index_string] = newcolname;
                }
                else
                {
                    newcolname=query->tables[i] + "." + cur_column;
                    mycomp = query->conditions[index_string];
                    query->conditions[index_string] = "";
                    stringstream comp;
                    comp << mycomp;
                    string token;
                    while(comp >> token)
                    {
                        if(token.compare(cur_column)==0)
                            query->conditions[index_string]+=newcolname+" ";
                        else
                            query->conditions[index_string]+=token+" ";
                    }
                }
            }
        }
        if(count!=1)
            error = true;
    }
    return query;
}


struct full_query* check_error(struct full_query* query, struct full_db* db)
{
    vector<string>::iterator it1;
    map<string, map<int, string> >::iterator it2;

    if(query->columns.size()==1 && query->columns[0].compare("*")==0)
    {
        query->columns.clear();
        query->functions.clear();
        string newcolumns="";
        map< string, vector< int > >::iterator colit;
        for(int i=0;i<((query->tables).size());i++)
        {
            for(colit=db->data[query->tables[i]].begin();colit!=db->data[query->tables[i]].end();colit++)
            {
                newcolumns="";
                newcolumns+=query->tables[i]+"."+colit->first;
                query->columns.push_back(newcolumns);
                query->functions.push_back("NULL");
            }
        }
    }
    // Check if tables mentioned in from part exist
    for(it1 = (query->tables).begin();it1!=(query->tables).end();it1++)
    {
        if((db->meta).find(*it1) == (db->meta).end())
            error = true;
    }

    // Check columns mentioned in select part exist in the given tables
    for(int i=0;i<(query->columns).size();i++)
    {
        string cur_column = query->columns[i];
        query = verify_wrapper(db, query, cur_column, i, true);
    }

    // Check columns mentioned in the where part (conditions)
    for(int i=0;i<(query->conditions).size();i++)
    {
        string cur_column;
        cur_column = query->conditions[i];
        stringstream comp;
        string token;
        comp << cur_column;
        int count = 0;
        while(comp>>token)
        {
            if(count==0 || count==2)
            {
                //   cout << "TOKEN: " << token << endl;
                if(!isnumber(token))
                    query = verify_wrapper(db, query, token, i, false);
            }
            count++;
            token="";
        }
    }
    return query;

}

void distinct_handler(struct full_miner* miner, struct full_query* query)
{
    int i;
    bool flag  = false;
    for(i=0;i<query->functions.size();i++)
        if(query->functions[i]=="distinct")
        {
            flag=true;
            break;
        }
    if(!flag)
        return;
    string col_name = query->columns[i];
    set<int> dis_col;
    int col_index = miner->m[col_name];
    for(i=0;i<join_table.size();i++)
    {
        if(dis_col.find(join_table[i][col_index]) == dis_col.end())
            dis_col.insert(join_table[i][col_index]);
        else
        {
            join_table.erase(join_table.begin()+i);
            i--;
        }
    }
}

int main(int argc, char* argv[])
{
    full_db* db = getMetaData();
    //    print_MetaData(db);
    populate_db(db);
    //  print_MetaData(db);
    string q="";
    //    for(int i=1;i<argc;i++)
    //  {
    //    q+=argv[i];
    //  q+=" ";
    // }
    // cout << q << endl;
    struct full_query * query=parse_query(argv[1]);
    query = fill_functions(query);
    query = check_error(query,db);
    if(error)
        cout << "error\n";
    else
    {
        if(func_handler(query->functions[0], db, query))
            return 0;
        struct full_miner * miner= prepare_mining(query,db);
        mine(miner,db, query);
      //  cout << "GOOD\n";
        distinct_handler(miner, query);
        select_from_db(miner,query);

    }
    /*    for(int i=0;i<query->columns.size();i++)
          cout<<query->columns[i]<<endl;
          cout<<endl;
          for(int i=0;i<query->tables.size();i++)
          cout<<query->tables[i]<<endl;
          cout<<endl;
          for(int i=0;i<query->conditions.size();i++)
          cout<<query->conditions[i]<<endl;
          cout<<endl;
          for(int i=0;i<query->operators.size();i++)
          cout<<query->operators[i]<<endl;
          cout<<endl;
          for(int i=0;i<query->functions.size();i++)
          cout<<query->functions[i]<<endl;
     */
//    cout<<endl;
    return 0;
}

