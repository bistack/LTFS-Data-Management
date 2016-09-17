#ifndef _INFOREQUESTS_H
#define _INFOREQUESTS_H

class InfoRequests : public Operation

{
private:
public:
    InfoRequests() : Operation("+hwn:") {};
    ~InfoRequests() {};
    void printUsage();
    void doOperation(int argc, char **argv);
    static constexpr const char *cmd1 = "info";
    static constexpr const char *cmd2 = "requests";
};

#endif /* _INFOREQUESTS_H */
