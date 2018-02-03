#ifndef COMMON_H
#define COMMON_H

#define safe_delete(p) delete p; (p)=nullptr
#define safe_array_delete(p) delete[] p; (p)=nullptr

#endif // COMMON_H
