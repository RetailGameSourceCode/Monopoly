//  This is the structure of the HMD header.  HMD is divided into multiple blocks,
// each block being a group of primitives linked together in a list.  The headers
// to each primitive is stored in a seperate section.
struct HMD_Header
{
  ULONG   version_id;
  ULONG   map_flag;
  ULONG*  primitive_header_section_point; // pointer to primitive
                                          // header section
  ULONG   num_blocks; // Number of sections/blocks.  
};

//  Each section/block pointer above points to a primitive.  A primitive
//  also has a pointer to a primitive header, right here. Each primitive has
//  associated with it a number of data sections which the header points to.
struct HMD_Primitive_Header
{
  ULONG   header_size;
  ULONG*  section_pointer;  // Array of section pointers, one after another.
              // if MSB is 1, interpreted as data, not a pointer (if not mapped.
              // if mapped, MSB 0 = data, 1 = location).
};


// The primitive list structure - each primitive points to another, creating
// a chain, with the last one ending in 0xFFFFFFFF.
struct HMD_Primitive_Struct
{
  HMD_Primitive_Struct* next_prim;   // Next primitive
  HMD_Primitive_Header* prim_header;  // Header for this primitive
  long                  num_data_sections;  // Number of data sections
};


//  The following is the structure of the animation primitive header.
struct HMD_Animation_Header
{
  ULONG   animation_header_size;  // Note: this is exactly the same value as the
                                // header size that comes just before this value
  ULONG*  interpolation_function_point;
  ULONG*  sequence_control_point;
  ULONG*  parameter_section_point;
  ULONG*  coordinate_point;
};


