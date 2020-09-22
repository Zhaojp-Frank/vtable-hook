# build as exec binary
g++ -o hookT ./example.cpp -Wno-pmf-conversions

# build as shared lib
#g++ -shared -fPIC -o libhookT.so ./example.cpp -Wno-pmf-conversions

#-Wno-pmf-conversions: if not set, warning: converting from ‘int (Object::)(int)’ to ‘fptr {aka int (*)(Object*)}’ [-Wpmf-conversions]

