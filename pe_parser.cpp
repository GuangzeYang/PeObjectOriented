#include <iostream>
#include <Windows.h>



class FileHeader{
    public:
        WORD machine;
        WORD number_of_sections;
        WORD size_of_optional_header;

        FileHeader(){};
        FileHeader(HANDLE fl_base){
            /* fl_base 为文件头基址 */

            // HANDLE 类型指针无法进行指针运算（因为它是void），故此需转换类型
            this->machine = *(WORD*)((BYTE*)fl_base + 0x0);
            this->number_of_sections = *(WORD*)((BYTE*)fl_base + 0x2);
            this->size_of_optional_header = *(WORD*)((BYTE*)fl_base + 0x10);
        }
};

class OptionalHeader{
    public:
        WORD magic;
        DWORD size_of_code;
        DWORD address_of_entry_point;
        DWORD base_of_code;
        ULONGLONG image_base;                // 0x18: 镜像基址（64位地址，如0x140000000）
        DWORD section_alignment;            // 0x20: 内存中节对齐
        DWORD file_alignment;               // 0x24: 文件中节对齐
        DWORD size_of_image;                 // 0x38: 镜像总大小（内存中）
        DWORD size_of_headers;               // 0x3C: 所有头大小（文件中）
        DWORD number_of_data_directory;      // 0x6C: 数据目录项数（16）

        OptionalHeader(){};
        OptionalHeader(HANDLE op_base){
        }
};

class ExportTable{
    private:
        DWORD   address_of_functions;   
        DWORD   address_of_names;       
        DWORD   address_of_name_ordinals;

    public:
        DWORD name;                 
        DWORD base;                 
        DWORD number_of_functions;    
        DWORD number_of_names;        
        DWORD *export_addr_table;
        DWORD *name_pointer_table;
        DWORD *name_ordinal;

        ExportTable(HANDLE export_base){
            this->name = *(DWORD*)((BYTE*)export_base + 0x0C);
            this->base = *(DWORD*)((BYTE*)export_base + 0x10);
            this->number_of_functions = *(DWORD*)((BYTE*)export_base + 0x14);
            this->number_of_names = *(DWORD*)((BYTE*)export_base + 0x18);
            this->address_of_functions = *(DWORD*)((BYTE*)export_base + 0x1C);
            this->address_of_names = *(DWORD*)((BYTE*)export_base + 0x20);
            this->address_of_name_ordinals = *(DWORD*)((BYTE*)export_base + 0x24);

            // 解析嵌套的表结构
            
        }
};

class ImportTable{
    public:
        
};

class NTHeader{
    public:
        DWORD signature;
        FileHeader file_header;
        OptionalHeader optional_header;

        NTHeader(){};
        NTHeader(HANDLE nt_base){
            /*
            nt_base: NT Header base address(dos_header->e_lfanew)
            */
            this->signature = *(DWORD*)nt_base;
            
            HANDLE fl_base = (BYTE*)nt_base + 0x4;
            this->file_header = FileHeader(fl_base);

            HANDLE op_base = (BYTE*)nt_base + 0x18;
            this->optional_header = OptionalHeader(op_base);
        }
};


class PEAnalyzer{
    private:
        HANDLE pe_file_handle;
        HANDLE pe_mapping;
        HANDLE pe_memory;
    public:
        // dos头 成员变量
        IMAGE_DOS_HEADER *dos_header;
        NTHeader nt_header;

        PEAnalyzer(std::string file_path){
            this->pe_file_handle = CreateFile(file_path.c_str(), GENERIC_READ,FILE_SHARE_READ,NULL,
                                                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (this->pe_file_handle == INVALID_HANDLE_VALUE) {
                std::cerr << "CreateFile failed, error=" << GetLastError() << "\n";
            }else{
                this->pe_mapping = CreateFileMapping(this->pe_file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
                this->pe_memory = MapViewOfFile(this->pe_mapping,FILE_MAP_READ, 0, 0, 0);
                std::cout<<std::hex<<*(WORD*)this->pe_memory<<std::endl;
            }
            if (*(WORD*)this->pe_memory != 0x5a4d){
                throw "Not PE File";
            }
            get_dos_header();
            get_nt_header();
            get_section_table();
        }

        void get_dos_header(){
            this->dos_header = (IMAGE_DOS_HEADER*)this->pe_memory; 
            std::cout<<std::hex<<this->dos_header->e_lfanew<<std::endl;
        }

        void get_nt_header(){
            HANDLE nt_base = (BYTE*)this->pe_memory + this->dos_header->e_lfanew;
            this->nt_header = NTHeader(nt_base);
            std::cout<<std::hex<<this->nt_header.signature<<std::endl;
        }

        void get_section_table(){
            
        }
};




int main(int argc, char const *argv[])
{
    PEAnalyzer pe_analyzer("C:/Windows/System32/notepad.exe");
    std::cout<<"DEMO:"<<std::hex<<pe_analyzer.nt_header.file_header.size_of_optional_header<<std::endl;
    return 0;
}
