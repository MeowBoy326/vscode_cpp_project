#include<filesystem>
#include<chrono>
#include<iostream>
#include<fstream>
#include<vector>
#include<ctime> 
#include<io.h>
using namespace std;


void testFileTraverse() {
    string dir = "./";
    filesystem::directory_iterator begin(dir);
    filesystem::directory_iterator end;
    cout << "���� " << dir << " �������ļ���Ŀ¼��ʾ:" << endl;
    for (auto it = begin; it != end; ++it) {
        //it->path() ��ȡ�ļ�filesystem::path ����
        string pathname = it->path().string();
        string name = it->path().filename().string();
        //�ж��ļ��Ƿ�ΪĿ¼
        if (filesystem::is_directory(it->status())) {
            cout << pathname << " is a directory!" << endl;
            continue;
        }
        cout << "�ļ�·��Ϊ��" << pathname << "\t" << "�ļ���Ϊ��" << name << endl;
    }
}


void recursionFileTraverse(ofstream &outfile,string dir) {
    filesystem::directory_iterator begin(dir);
    filesystem::directory_iterator end;
    cout << "���� " << dir << " �������ļ���Ŀ¼��ʾ:" << endl;
    for (auto it = begin; it != end; ++it) {
        //it->path() ��ȡ�ļ�filesystem::path ����
        string pathname = it->path().string();
        string name = it->path().filename().string();
        //�ж��ļ��Ƿ�ΪĿ¼
        if (filesystem::is_directory(it->status())) {
            cout << pathname << " is a directory!" << endl;
            // if(pathname.compare("./.git") != 0)
            if(pathname.find(".git") == std::string::npos)
                recursionFileTraverse(outfile,pathname);
            continue;
        }
        outfile << pathname <<endl;
        cout << "�ļ�·��Ϊ��" << pathname << "\t" << "�ļ���Ϊ��" << name << endl;
    }
}


void testFileLastTime() {    
    filesystem::path  file = "./Filesystem.cc";
    int64_t fsize = filesystem::file_size(file);
    auto mtime = filesystem::last_write_time(file);
    //time_t ftime = file_time_type::clock::to_time_t(filesystem::last_write_time("./a.cc"))
    cout << "�ļ���СΪ��" << to_string(fsize) << endl;
    //cout << "�ļ�����޸�ʱ���Ϊ��" << to_string(ftime) << endl;
    //time_t cftime = decltype(mtime)::clock::to_time_t(mtime);
    // auto cftime = chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(mtime));
    //cout << "�ļ�����޸�ʱ���Ϊ��" << asctime(localtime(&cftime)) << endl;
    // struct tm* stm_ptr;
    // stm_ptr = localtime(&cftime);
    // char buf[1024];
    // strftime(buf, 1024, "%Y-%m-%d %H:%M:%S", stm_ptr);
    // cout << "�ļ�����޸�ʱ��Ϊ��" << buf << endl;
}

void testFilesystem(){
    cout << "Hello, c++17 filesystem!" <<endl;

   string abs_path1 = ".\\stable-diffusion.cpp";
   string abs_path2 = ".\\stable-diffusion.cpp\\CMakeLists.txt";
   string abs_path3 = ".\\stable-diffusion.cpp\\thirdparty";
   string abs_path4 = ".\\stable-diffusion.cpp\\thirdparty\\";
   string abs_path5 = ".\\stable-diffusion.cpp\\ggml";
   string abs_path6 = ".\\stable-diffusion.cpp\\ggml\\";

    // 1. path
    filesystem::path path1(abs_path1);
    filesystem::path path2(abs_path2);
    filesystem::path path3(abs_path3);
    filesystem::path path4(abs_path4);
    filesystem::path path5(abs_path5);
    filesystem::path path6(abs_path6);
   cout << "����ַ�����ʽ��·��: " << path1.string() <<endl; // ./filesystem_demo
   cout << "�ļ�����������չ��: " << path1.stem() <<endl; // "filesystem_demo"
   cout << "�����ļ���չ��: " << path1.extension() <<endl; // ""
   cout << "��ȡ�ļ���Ŀ¼�ľ���·��: " << filesystem::absolute(path1) <<endl; // ./filesystem_demo
   cout << "��ǰ·��: " << filesystem::current_path() <<endl; // ���û�в������룬�򷵻ص�ǰ����Ŀ¼�������򽫴����Ŀ¼��Ϊ��ǰ����Ŀ¼
   cout << "ƴ��·��1: " << path5/"jjjj" <<endl;
   cout << "ƴ��·��2: " << path6/"jjjj" <<endl;


    // 2. �ж��ļ�
    auto status = filesystem::status(path1);
   // cout << "�ļ�״̬-Ȩ��: " << status.permissions() <<endl;
   // cout << "�ļ�״̬-����: " << status.type() <<endl;

   cout << "�ж�-Ŀ¼�Ƿ����: " << filesystem::is_directory(path1) <<endl; // 1
   cout << "�ж�-Ŀ¼�Ƿ����: " << filesystem::is_directory(path2) <<endl; // 0
   cout << "�ж�-Ŀ¼�Ƿ�Ϊ��: " << filesystem::is_empty(path1) <<endl; // 0
   cout << "�ж�-Ŀ¼�Ƿ�Ϊ��: " << filesystem::is_empty(path2) <<endl; // 0
   cout << "�ж�-�Ƿ�Ϊ�ļ�: " << filesystem::is_regular_file(path1) <<endl; // 0
   cout << "�ж�-�Ƿ�Ϊ�ļ�: " << filesystem::is_regular_file(path2) <<endl; // 1
   cout << "�ж�-�Ƿ�Ϊ�����ļ�: " << filesystem::is_symlink(path1) <<endl; // 0
   cout << "�ж�-�Ƿ�Ϊ�����ļ�: " << filesystem::is_symlink(path2) <<endl; // 0
   cout << "�ж�-�Ƿ����: " << filesystem::exists(path1) <<endl; // 1
   cout << "�ж�-�Ƿ����: " << filesystem::exists(path2) <<endl; // 1
//   exceptioncout << "�ļ���С: " << filesystem::file_size(path1) <<endl; // exception
   cout << "�ļ���С: " << filesystem::file_size(path2) <<endl; // 2961 bytes
   // cout << "����޸�ʱ��: " << filesystem::last_write_time(path1) <<endl; // 1640914873 ʱ���
   // cout << "����޸�ʱ��: " << filesystem::last_write_time(path2) <<endl; // 1640914873 ʱ���

    // 3. �����ļ�
    filesystem::create_directory("C:\\Logs");
    filesystem::create_directories("C:\\Log\\aa\\bb\\cc");
    // filesystem::create_directory(path3); // ����Ŀ¼
    // filesystem::create_directories(path3); // ����Ŀ¼
    // filesystem::create_directories(path4); // ����Ŀ¼

//    filesystem::remove(path3); // ɾ��Ŀ¼
   cout << "-----ɾ�� " <<endl;
    string abs_remove_all_path = "C:\\Log";
    filesystem::path remove_all_path(abs_remove_all_path);
    filesystem::remove_all(remove_all_path); // �ݹ�ɾ��Ŀ¼
   cout << "-----ɾ�� " <<endl;

//    filesystem::rename(path3,path5); // ������Ŀ¼(Ŀ¼����ʱ)

//    filesystem::copy_file() // �����ļ�
//    filesystem::copy_directory() // ����Ŀ¼


//    // 4. �����ļ���
    filesystem::directory_iterator end_iter1; // ������ǰĿ¼
    for(filesystem::directory_iterator file_iter(path1); file_iter != end_iter1; ++file_iter ) {
       cout << "file: " << *file_iter  <<endl;
    }
   cout << "-------" <<endl;
    filesystem::recursive_directory_iterator end_iter2; // �ݹ�ر���һ��Ŀ¼����Ŀ¼��Ҳ���ǵ�������Ŀ¼�ṹ�µ������ļ�
    for(filesystem::recursive_directory_iterator file_iter(path1); file_iter != end_iter2; ++file_iter ) {
       cout << "file: " << *file_iter  <<endl;
    }

    // 5. �ļ���
    // // ͷ�ļ�<fstream>������ļ������ܽ�filesystem::path�����Ŀ¼��Ϊ�����������Ҫʹ��path������ļ����Ļ����Ǿ����ͷ�ļ�<boost/filesystem/fstream.hpp>
    filesystem::path p{"haha.txt"};
    ofstream ofs{p};
    ofs << "Hello, world!\n";   
}


// vector<filesystem::path> myDirectoryIterator(char * filePath)
// {
//     vector<filesystem::path> file_list;
//     for (auto& i : filesystem::directory_iterator("D:\\WorkZone\\")) {
//         cout << i.path().string()<< endl;
//         if(filesystem::is_directory(i))
//         {
//             file_list.push_back(i.path());
//         }else{
//             vector<filesystem::path> tmp = myDirectoryIterator(i.path().c_str());
//             file_list.insert(file_list.end(), tmp.begin(), tmp.end());
//         }
//     }
//     return file_list;
// }

/// <summary>
/// (�ݹ�,�ǵݹ�) �Ļ�ȡָ��Ŀ¼�����е�(�ļ�����Ŀ¼)��(����·�������ļ�����)
/// </summary>
/// <param name="directoryPath">ָ��Ŀ¼</param>
/// <param name="isFile">true:�ļ���false:Ŀ¼</param>
/// <param name="returnFullPath">true��ʾ����·����false����ʾ����</param>
/// <param name="recursive">false:�ǵݹ�,true:�ݹ�</param>
/// <returns></returns>
std::vector<std::string> collectFileOrDirEntries(const std::string& directoryPath, bool isFile = true, bool returnFullPath = true, bool recursive = false)
{
    std::vector<std::string> entries;
    try
    {
        std::filesystem::path dirPath(directoryPath);
        if (recursive)  // �ݹ�
        {
            for (auto& entry : std::filesystem::recursive_directory_iterator(dirPath))
            {
                if ((isFile && entry.is_regular_file()) || (!isFile && entry.is_directory()))
                {
                    if (returnFullPath)
                    {
                        entries.push_back(entry.path().string());
                    }
                    else
                    {
                        entries.push_back(entry.path().filename().make_preferred().string());
                    }
                }
            }
        }
        else // �ǵݹ�
        {
            for (auto& entry : std::filesystem::directory_iterator(dirPath))
            {
                if ((isFile && entry.is_regular_file()) || (!isFile && entry.is_directory()))
                {
                    if (returnFullPath)
                    {
                        entries.push_back(entry.path().string());
                    }
                    else
                    {
                        entries.push_back(entry.path().filename().make_preferred().string());
                    }
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cout << "Error accessing directory: " << e.what() << std::endl;
    }
    return entries;
}


int main() {
    //io.h �����ļ���
    _finddata64i32_t fileInfo;
    intptr_t hFile=_findfirst("./*",&fileInfo);
    if (hFile == -1) {
        return -1;
    }
    do
    {
        cout << fileInfo.name << endl;
    } while (_findnext(hFile,&fileInfo)==0);
    
    //filesystem �����ļ���
    for (auto& i : filesystem::directory_iterator("./")) {
        cout << i.path().string()<< endl;
        if(filesystem::is_directory(i))
        {
            
        }
    }
    
    //filesystem �����ļ����Լ����ļ���
    // ����һ��vector���洢Ŀ¼�е��ļ��б�
    vector<filesystem::path> file_list;
    for (auto& i : filesystem::recursive_directory_iterator ("./")) {
        cout << i.path().string()<< endl;
    }
    
    
    testFilesystem();

    // ���ļ��б��������
    // ����һ��vector���洢Ŀ¼�е��ļ��б�
    // vector<filesystem::path> file_list;
    // filesystem::directory_iterator end_itr;
    // for (filesystem::directory_iterator itr("./"); itr != end_itr; ++itr)
    // {
    //     if (filesystem::is_regular_file(itr->status()))
    //     {
    //         file_list.push_back(itr->path());
    //     }
    // }
    // ��file_list��������
    // sort(file_list.begin(), file_list.end());
    // for (filesystem::path i : file_list) {
    //     cout << i.string()<< endl;
    // }
    
     testFileTraverse();
     testFileLastTime();
     
    ofstream outfile("Path.txt", ios::out);
    if (outfile) {
        recursionFileTraverse(outfile, "./");
        outfile.close();
        ifstream infile("Path.txt", ios::in);
        string line;
        if (infile.is_open()) {
            while (getline(infile, line)) {
                cout << line << endl;
            }
            infile.close();
        } else {
            cout << "Failed to open the file." << endl;
        }
	}else{
		cout << "Failed to write the file." << endl;
	}
   
     
    
     
    // ����Ŀ¼�µ������ļ�
    // for (const auto& entry :filesystem::directory_iterator(m_imgPath))
    // {
    //     // ������ļ���������ļ���
    //     /*if (entry.is_regular_file())
    //     {
    //         m_imgFilelist.push_back(entry.path().filename().string());
    //     }*/
    //     // add file path
    //     if (entry.path().extension() == ".png")
    //     {
    //         m_imgFilelist.push_back(m_imgPath + entry.path().filename().string());
    //     }
    // }
    return 0;
}