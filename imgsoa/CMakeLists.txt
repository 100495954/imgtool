add_library(imgsoa
        imagesoa.cpp)
target_link_libraries(imgsoa PUBLIC common PRIVATE Microsoft.GSL::GSL)