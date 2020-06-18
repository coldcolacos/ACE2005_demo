#include<bits/stdc++.h>
using namespace std;

template<class T = unsigned char, int SYM_NUM = 256>
struct HuffmanTree{
    struct Node{
        bool is_leaf;
        T symbol;
        int next[2];
        long long occur_num;

        Node(){
            is_leaf = false;
            occur_num = 0;
            next[0] = next[1] = -1;
        }

        bool operator<(Node& other) const {
            return occur_num > other.occur_num;
        }
    };

    Node tree[SYM_NUM * 2];

    int root, node_num;
    int new_node(){
        return node_num ++;
    }


    HuffmanTree(map<T, long long>& occur_num_of_char){

        node_num = 0;
        int char_num = occur_num_of_char.size();

        if(char_num == 1){
            root = new_node();
            int now = new_node();
            tree[now].is_leaf = true;
            tree[now].symbol = occur_num_of_char.begin() -> first;
            tree[root].next[0] = now;
        }else{
            priority_queue<pair<long long, int>> Q;
            for(auto& c : occur_num_of_char){
                int now = new_node();
                tree[now].is_leaf = true;
                tree[now].symbol = c.first;
                tree[now].occur_num = c.second;
                Q.push(make_pair(- c.second, now));
            }
            while(Q.size() > 1){
                auto a = Q.top();
                Q.pop();
                auto b = Q.top();
                Q.pop();

                int now = new_node();
                tree[now].occur_num = a.first + b.first;
                tree[now].next[0] = a.second;
                tree[now].next[1] = b.second;

                Q.push(make_pair(tree[now].occur_num, now));
            }
            root = Q.top().second;
        }

        vector<bool> tmp;
        get_path(root, 0, tmp);
    }

    vector<bool> path[SYM_NUM];

    void get_path(int now, int dep, vector<bool>& _path){
        if(tree[now].is_leaf){
            path[tree[now].symbol] = _path;
            /// cout<< tree[now].symbol << ' ' << dep << endl;
        }
        for(int i = 0; i < 2; i ++){
            if(tree[now].next[i] != -1){
                _path.push_back(i);
                get_path(tree[now].next[i], dep + 1, _path);
                _path.pop_back();
            }
        }
    }

    vector<bool> encode(vector<T> s){
        vector<bool> res;
        for(auto& c : s){
            for(auto b : path[c]){
                res.push_back(b);
            }
        }
        return res;
    }

    string decode(string s){
        int now = root;
        string res = "";
        for(auto c : s){
            int dir = c - '0';
            now = tree[now].next[dir];
            if(tree[now].is_leaf){
                res += tree[now].symbol;
                now = root;
            }
        }
        return res;
    }
};

struct HuffDict{

    const unsigned LOW_16 = (1u << 16) - 1;
    const unsigned HIGH_16 = LOW_16 << 16;
    HuffmanTree<>* tree;
    int kv_num;

    struct KeyNode{
        unsigned short key_len, value_len;
        unsigned int code, key_begin, value_begin;
        bool operator<(const KeyNode& other) {
            return code < other.code;
        }
    };
    vector<vector<KeyNode>>* key_hash_table;
    vector<unsigned char>* key_huff_bits;
    vector<unsigned char>* value_huff_bits;

    void clear(){
        kv_num = 0;
        if(tree) delete tree;
        if(key_hash_table) delete key_hash_table;
        if(key_huff_bits) delete key_huff_bits;
        if(value_huff_bits) delete value_huff_bits;
    }

    HuffDict(){
        tree = nullptr;
        key_hash_table = nullptr;
        key_huff_bits = nullptr;
        value_huff_bits = nullptr;
    }
    ~HuffDict() {clear();}

    void load_from_kv_file(const char* fpath, unsigned char separator = ','){

        clear();

        map<unsigned char, long long> occur_num_of_char;
        FILE* file = fopen(fpath, "r");
        while(char ch = fgetc(file)){
            if(ch == EOF) break;
            if(ch == '\n'){
                kv_num ++;
                continue;
            }
            if(ch != separator) occur_num_of_char[ch] ++;
        }
        tree = new HuffmanTree<>(occur_num_of_char);

        fseek(file, 0, SEEK_SET);
        key_hash_table = new vector<vector<KeyNode>>(1 << 16);
        key_huff_bits = new vector<unsigned char>;
        value_huff_bits = new vector<unsigned char>;
        unsigned int key_ptr = -1, value_ptr = -1, _key_ptr, _value_ptr;
        for(int i = 0; i < kv_num; i ++){
            _key_ptr = key_ptr;
            unsigned int hash_code = 0, verify_code = 0;
            while(char ch = fgetc(file)){
                if(ch == separator) break;
                hash_code = hash_code * 131 + ch;
                verify_code = verify_code * 31 + ch;
                for(auto b : tree->path[ch]){
                    _key_ptr ++;
                    if((_key_ptr& 7) == 0){
                        key_huff_bits->push_back(0);
                    }
                    if(b) (*key_huff_bits)[_key_ptr / 8] |= (1u << (_key_ptr & 7));
                }
            }

            _value_ptr = value_ptr;
            while(char ch = fgetc(file)){
                if(ch == '\n') break;
                for(auto b : tree->path[ch]){
                    _value_ptr ++;
                    if((_value_ptr& 7) == 0){
                        value_huff_bits->push_back(0);
                    }
                    if(b) (*value_huff_bits)[_value_ptr / 8] |= (1u << (_value_ptr & 7));
                }
            }

            auto tmp = (KeyNode){_key_ptr - key_ptr, _value_ptr - value_ptr, hash_code & LOW_16 ^ verify_code, key_ptr + 1, value_ptr + 1};
            (*key_hash_table)[hash_code & HIGH_16 >> 16].push_back(move(tmp));

            key_ptr = _key_ptr | 7;
            value_ptr = _value_ptr | 7;
        }

        for(int i = 0; i < LOW_16; i ++){
            auto& v = (*key_hash_table)[i];
            if(!v.empty()) sort(v.begin(), v.end());
        }
    }

    string decode(vector<unsigned char>* bits, unsigned long long begin, unsigned long long len){
        string res = "";
        int now = tree->root;
        for(auto i = begin; i < begin + len; i ++){
            now = tree->tree[now].next[(*bits)[i / 8] >> (i & 7) & 1];
            if(tree->tree[now].is_leaf){
                res += tree->tree[now].symbol;
                now = tree->root;
            }
        }
        return res;
    }

    int search(const char* s, string& result){
        int slen = strlen(s);
        unsigned int hash_code = 0, verify_code = 0;
        vector<unsigned char>* bits = new vector<unsigned char>;
        unsigned int ptr = -1;
        for(int i = 0; i < slen; i ++){
            hash_code = hash_code * 131 + s[i];
            verify_code = verify_code * 31 + s[i];
            for(auto b : tree->path[s[i]]){
                ptr ++;
                if((ptr& 7) == 0){
                    bits->push_back(0);
                }
                if(b) (*bits)[ptr / 8] |= (1u << (ptr & 7));
            }
        }

        auto& v = (*key_hash_table)[hash_code & HIGH_16 >> 16];
        KeyNode tmp;
        tmp.code = hash_code & LOW_16 ^ verify_code;
        auto pos = lower_bound(v.begin(), v.end(), tmp);
        for(; pos != v.end() && pos->code == tmp.code; pos ++){
            unsigned int begin = pos->key_begin;
            unsigned int len = (pos->key_len + 7) / 8;
            bool ok = true;
            for(int i = 0; i < len; i ++){
                if((*key_huff_bits)[i + begin / 8] != (*bits)[i]){
                    ok = false;
                    break;
                }
            }
            if(ok){
                result = decode(value_huff_bits, pos->value_begin, pos->value_len);
                return 1;
            }
        }
        result = "";
        return 0;
    }

    void check(){
        for(auto& hash_list : *key_hash_table){
            for(auto& key_node : hash_list){
                puts("---");
                cout<< key_node.key_begin<< ' ' << key_node.key_len<<endl;
                cout<< decode(key_huff_bits, key_node.key_begin, key_node.key_len) << endl;
                cout<< decode(value_huff_bits, key_node.value_begin, key_node.value_len) << endl;
            }
        }
    }
};

int main(){
    HuffDict hd;
    hd.load_from_kv_file("in.cpp");

    freopen("query.cpp", "r", stdin);
    freopen("res", "w", stdout);
    string key, result;
    while(cin >> key){
        int ok = hd.search(key.c_str(), result);
        cout<< ok << ' ' << result << endl;
    }
}
