#include <iostream>
#include <mariadb/conncpp.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

class Search
{
private:
    std::string keyword1;
    std::string keyword2;
    std::string keyword3;
    std::vector<std::string> history;

public:
    void addToHistory(const std::string &keyword)
    {
        time_t now = time(0);
        tm *settime = localtime(&now);
        char times[20];
        strftime(times, sizeof(times), "%Y/%m/%d %H:%M:%S", settime);
        history.push_back(std::string(times) + " - " + keyword);
        if (history.size() > 30)
            history.erase(history.begin());
    }

    void showHistory()
    {
        std::cout << "히스토리 검색:" << std::endl;
        for (int i = 0; i < history.size(); ++i)
        {
            std::cout << i + 1 << ": " << history[i] << std::endl;
        }
    }

    void searchAndShow(std::unique_ptr<sql::Connection> &conn, const std::string &keyword)
    {
        std::cout << "검색 결과: " << keyword << std::endl;
        if (keyword.find("PROPERTY") != std::string::npos)
        {
            showPropertys(conn);
        }
        else if (keyword.find("SUBWAY") != std::string::npos)
        {
            showSubway(conn);
        }
        else if (keyword.find("MART") != std::string::npos)
        {
            showMarts(conn);
        }
        else if (keyword.find("PARK") != std::string::npos)
        {
            showParks(conn);
        }
        else if (keyword.find("BUILDINGS") != std::string::npos)
        {
            ShowRealestate(conn); // 건물 정보 검색
        }
        else
        {
            std::cout << "올바르지 않은 키워드입니다." << std::endl;
        }
    }

    void searchFromHistory(std::unique_ptr<sql::Connection> &conn)
    {
        int index;
        showHistory();
        std::cout << "재검색할 번호를 선택 : ";
        std::cin >> index;

        if (index >= 1 && index <= history.size())
        {
            std::string historyEntry = history[index - 1];
            size_t pos = historyEntry.find(" - ");
            if (pos != std::string::npos)
            {
                // std::string keyword = historyEntry.substr(pos + 3); // 키워드 추출
                std::string keyword = historyEntry.substr(pos);
                // 검색 결과를 먼저 보여줌
                std::cout << "검색 결과: " << historyEntry << std::endl;
                // 사용자가 다시 선택하지 않고 해당 히스토리의 검색어를 그대로 사용
                searchAndShow(conn, keyword);
            }
        }
        else
        {
            std::cout << "알맞는 번호를 골라주세요" << std::endl;
        }
    }

    void showParks(std::unique_ptr<sql::Connection> &conn)
    {
        try
        {
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            int i = 1;
            sql::ResultSet *res = stmnt->executeQuery("select* from PARK");
            std::cout << "[ 지역구: " << keyword2;
            std::cout << "/ 법정동: " << keyword3 << " ]" << std::endl;
            while (res->next())
            {
                if (keyword2.compare(res->getString(2)) == 0 && keyword3.compare(res->getString(3)) == 0)
                {
                    std::cout << i++ << " : " << res->getString(4);
                    std::cout << "\n";
                }
                else
                    continue;
            }
            addToHistory("PARK 검색 - " + keyword2 + " / " + keyword3); // 히스토리에 검색 내용 추가
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
    }

    void showPropertys(std::unique_ptr<sql::Connection> &conn)
    {
        try
        {
            int i = 1;
            std::cout << "[ 지역구: " << keyword2;
            std::cout << "/ 법정동: " << keyword3 << " ]" << std::endl;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::ResultSet *res = stmnt->executeQuery("select* from PROPERTY");
            while (res->next())
            {
                if (keyword2.compare(res->getString(4)) == 0 && keyword3.compare(res->getString(5)) == 0)
                {
                    std::cout << i++ << " : " << res->getString(9);
                    std::cout << "\n";
                }
                else
                    continue;
            }
            addToHistory("PROPERTY 검색 - " + keyword2 + " / " + keyword3); // 히스토리에 검색 내용 추가
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
    }

    void showMarts(std::unique_ptr<sql::Connection> &conn)
    {
        try
        {
            int i = 1;
            std::cout << "[ 지역구: " << keyword2;
            std::cout << "/ 법정동: " << keyword3 << " ]" << std::endl;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::ResultSet *res = stmnt->executeQuery("select* from MART");
            while (res->next())
            {

                if (keyword2.compare(res->getString(2)) == 0 && keyword3.compare(res->getString(3)) == 0)
                {
                    std::cout << i++;
                    std::cout << " : " << res->getString(4);
                    std::cout << " ( " << res->getString(5) << " )";
                    std::cout << "\n";
                }
                else
                    continue;
            }
            addToHistory("MART 검색 - " + keyword2 + " / " + keyword3); // 히스토리에 검색 내용 추가
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
    }

    void ShowRealestate(std::unique_ptr<sql::Connection> &conn)
    {
        int i = 1;
        try
        {
            std::vector<std::string> check;
            std::vector<std::string> check2;
            std::vector<std::string> check3;

            std::cout << "검색 가능한 주거 형태는 다음과 같습니다." << std::endl;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::ResultSet *res = stmnt->executeQuery("select * from BUILDINGS");
            while (res->next())
            {
                if (find(check.begin(), check.end(), res->getString(2)) == check.end())
                    check.emplace_back(res->getString(2));
            }

            std::cout << "[ 조회 원하시는 것을 입력 부탁드립니다 ]" << std::endl;
            std::cout << "========================================================================================" << std::endl;
            for (int i = 0; i < check.size(); i++)
            {
                std::cout << check.at(i) << ' ';
                if (i % 10 == 0)
                {
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl
                      << std::endl
                      << "========================================================================================" << std::endl;
            std::cout << " 입력 : ";
            std::cin >> keyword1;
            res = stmnt->executeQuery("select * from BUILDINGS");
            while (res->next())
            {
                if (res->getString(2) == keyword1)
                {
                    if (find(check2.begin(), check2.end(), res->getString(5)) == check2.end())
                        check2.emplace_back(res->getString(5));
                }
            }
            std::cout << "[ 조회 원하시는 것을 입력 부탁드립니다 ]" << std::endl;
            std::cout << "========================================================================================" << std::endl;
            for (int i = 0; i < check2.size(); i++)
            {
                std::cout << check2.at(i) << ' ';
                if ((i + 1) % 10 == 0)
                {
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl
                      << "========================================================================================" << std::endl;
            std::cout << " 입력 : ";
            std::cin >> keyword2;
            res = stmnt->executeQuery("select * from BUILDINGS");
            while (res->next())
            {
                if (res->getString(2) == keyword1 && res->getString(5) == keyword2)
                {
                    if (find(check3.begin(), check3.end(), res->getString(6)) == check3.end())
                        check3.emplace_back(res->getString(6));
                }
            }
            std::cout << "[ 조회 원하시는 것을 입력 부탁드립니다 ]" << std::endl;
            std::cout << "========================================================================================" << std::endl;
            for (int i = 0; i < check3.size(); i++)
            {
                std::cout << check3.at(i) << ' ';
                if ((i + 1) % 10 == 0)
                {
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl
                      << "========================================================================================" << std::endl;
            std::cout << " 입력 : ";
            std::cin >> keyword3;
            res = stmnt->executeQuery("select * from BUILDINGS");
            std::cout << "입력하신 지역의 (" << keyword1 << ") 최근 거래 정보입니다." << std::endl;
            std::cout << "[ 지역구: " << keyword2;
            std::cout << "/ 법정동: " << keyword3 << " ]" << std::endl;
            std::cout << "========================================================================================" << std::endl;
            while (res->next())
            {
                if (keyword1.compare(res->getString(2)) == 0 && keyword2.compare(res->getString(5)) == 0 && keyword3.compare(res->getString(6)) == 0)
                {
                    std::cout << i++;
                    std::cout << " : " << res->getString(9) << " " << res->getInt(13) << " 층";
                    std::cout << " [ " << res->getInt(11) << " 만원 / " << res->getInt(14) << "년 준공 ]";
                    std::cout << "\n";
                }
            }
            std::cout << "========================================================================================" << std::endl;
            addToHistory("BUILDINGS 검색 - " + keyword1 + " / " + keyword2 + " / " + keyword3); // 히스토리에 검색 내용 추가
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
    }

    void showSubway(std::unique_ptr<sql::Connection> &conn)
    {
        try
        {
            int i = 1;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::ResultSet *res = stmnt->executeQuery("select * from SUBWAY");
            while (res->next())
            {
                if (keyword2.compare(res->getString(5)) == 0 && keyword3.compare(res->getString(7)) == 0)
                {
                    std::cout << i++;
                    std::cout << " : " << res->getInt(2) << "호선 [" << res->getString(3) << "역]";
                    std::cout << "\n";
                }
                else
                    continue;
            }
            addToHistory("SUBWAY 검색 - " + keyword2 + " / " + keyword3); // 히스토리에 검색 내용 추가
        }

        catch (sql::SQLException &e)
        {
            std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
    }
    void Strat(std::unique_ptr<sql::Connection> &conn)
    {
        ShowRealestate(conn);
        int count = 0;
        char sel;
        std::cout << " 매물 인근의 편의 시설을 검색할 수 있습니다." << std::endl
                  << "원하는 편의 시설을 선택해 주세요." << std::endl;
        std::cout << "0 : 처음으로\n1 : 부동산\n2 : 지하철\n3 : 마트\n4 : 공원\nQ : 종료" << std::endl;
        std::cin >> sel;
        while (count < 1)
        {
            switch (sel)
            {
            case '0':
                count++;
                break;

            case '1':
                system("clear");
                showPropertys(conn);
                count++;
                break;

            case '2':
                system("clear");
                showSubway(conn);
                count++;
                break;

            case '3':
                system("clear");
                showMarts(conn);
                count++;
                break;

            case '4':
                system("clear");
                showParks(conn);
                count++;
                break;

            case 'q':
            case 'Q':
                std::cout << "<system : 프로그램을 종료하겠습니다>";
                exit(1);
                break;

            default:
                std::cout << "올바른 번호를 선택해 주세요.";
                continue;
            }
        }
    }
};

class parkmart
{
public:
    void parkmarts(std::unique_ptr<sql::Connection> &conn)
    {
        std::string STR;
        try
        {
            int i = 1;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::SQLString select = "SELECT PARKNAME, workplace FROM PARK INNER JOIN MART ON PARK.DISTRICTNAME = MART.DISTRICTNAME WHERE MART.DISTRICTNAME = ";
            std::cout << "동작구 구로구 강서구 동대문구 노원구 중랑구 강북구 은평구 영등포구 용산구";
            std::cout << "관악구 도봉구 서대문구 성동구 금천구 강동구 성북구 양천구 중구 송파구";
            std::cout << "광진구 서초구 강남구 마포구 종로구" << std::endl;
            std::cout << "원하는 지역을 입력하세요 : ";
            std::cin >> STR;
            sql::ResultSet *parkmarts = stmnt->executeQuery(select + "\'" + STR + "\'" + "GROUP BY PARKNAME");
            while (parkmarts->next())
            {
                std::cout << i++;
                std::cout << " : " << parkmarts->getString(1);
                std::cout << " ( " << parkmarts->getString(2) << " )";
                std::cout << "\n";
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
    }
};

class homepark
{
public:
    void hmpark(std::unique_ptr<sql::Connection> &conn)
    {
        std::string STR;
        try
        {
            int i = 1;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::SQLString select = "SELECT PARKNAME, SUBNAME FROM PARK NATURAL JOIN PROPERTY NATURAL JOIN SUBWAY WHERE DISTRICTNAME = ";
            std::cout << "동작구 구로구 강서구 동대문구 노원구 중랑구 강북구 은평구 영등포구 용산구";
            std::cout << "관악구 도봉구 서대문구 성동구 금천구 강동구 성북구 양천구 중구 송파구";
            std::cout << "광진구 서초구 강남구 마포구 종로구" << std::endl;
            std::cout << "원하는 지역을 입력하세요 : ";
            std::cin >> STR;
            sql::ResultSet *hmpark = stmnt->executeQuery(select + "\'" + STR + "\'" + "GROUP BY PARKNAME");
            while (hmpark->next())
            {
                std::cout << i++;
                std::cout << " : " << hmpark->getString(1);
                std::cout << " : " << hmpark->getString(2) << "역";
                std::cout << "\n";
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
    }
};

class monapt
{
public:
    void moneyapt(std::unique_ptr<sql::Connection> &conn)
    {
        int STR;
        try
        {
            int i = 1;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::SQLString select = "SELECT STRUCTNAME, DISTRICTNAME FROM BUILDINGS WHERE MONEY < ";
            std::cout << "소유 금액을 입력하세요 : ";
            std::cin >> STR;
            std::string s = std::to_string(STR);
            sql::ResultSet *moneyapt = stmnt->executeQuery(select + s + " AND STRUCT = \"아파트\" ORDER BY DISTRICTNAME");
            while (moneyapt->next())
            {
                std::cout << i++;
                std::cout << " : " << moneyapt->getString(1);
                std::cout << " : " << moneyapt->getString(2);
                std::cout << "\n";
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
    }
};

class subwa
{
public:
    void subway(std::unique_ptr<sql::Connection> &conn)
    {
        int STR;
        try
        {
            int i = 1;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::SQLString select = "SELECT SUBNO, SUBNAME FROM SUBWAY WHERE SUBNO = ";
            std::cout << "몇호선을 검색하시겠습니까? (1~9): ";
            std::cin >> STR;
            std::string s = std::to_string(STR);
            std::cout << s << std::endl;

            sql::ResultSet *subway = stmnt->executeQuery(select + "\'" + s + "\'");
            while (subway->next())
            {
                std::cout << subway->getString(1);
                std::cout << " 호선 : " << subway->getString(2);
                std::cout << "\n";
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
    }
};

class buildinghigh
{
public:
    void builhigh(std::unique_ptr<sql::Connection> &conn)
    {
        std::string STR;
        try
        {
            int i = 1;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::SQLString select = "SELECT DISTRICTNAME, STRUCTNAME, HIGH FROM BUILDINGS WHERE HIGH IN (SELECT HIGH FROM BUILDINGS) AND STRUCT = ";
            std::cout << "연립다세대, 오피스텔, 아파트, 단독다가구 " << std::endl;
            std::cout << "원하는 건물 종류를 입력하세요 : ";
            std::cin >> STR;
            sql::ResultSet *builhigh = stmnt->executeQuery(select + "\'" + STR + "\'" + " ORDER BY HIGH DESC LIMIT 10");
            while (builhigh->next())
            {
                std::cout << i++;
                std::cout << " : " << builhigh->getString(1);
                std::cout << " : " << builhigh->getString(2);
                std::cout << " : " << builhigh->getString(3) << "층";
                std::cout << "\n";
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
    }
};
int main()
{
    try
    {
        Search Searching;
        parkmart parkmart;
        homepark homepark;
        monapt monapt;
        subwa subwa;
        buildinghigh building;
        sql::Driver *driver = sql::mariadb::get_driver_instance();
        sql::SQLString url("jdbc:mariadb://LOCALHOST/TEST");
        sql::Properties properties({{"user", "ADBZA"}, {"password", "1"}});
        std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));

        while (1)
        {
            char choice;
            std::cout << "1: 검색, 2: 히스토리 보기, 3: 히스토리 검색, 4: 공원,마트 검색, 5: 지하철, 공원 검색" << std::endl;
            std::cout << "6: 소유 금액별 부동산 조회, 7: 지하철 검색, 8 : 고층건물 검색 q: 종료" << std::endl;
            std::cout << "번호 선택: ";
            std::cin >> choice;

            switch (choice)
            {
            case '1':
                Searching.Strat(conn);
                break;
            case '2':
                Searching.showHistory();
                break;
            case '3':
                Searching.searchFromHistory(conn);
                break;
            case '4':
                parkmart.parkmarts(conn);
                break;
            case '5':
                homepark.hmpark(conn);
                break;
            case '6':
                monapt.moneyapt(conn);
                break;
            case '7':
                subwa.subway(conn);
                break;
            case '8':
                building.builhigh(conn);
                break;
            case 'q':
            case 'Q':
                std::cout << "종료합니다." << std::endl;
                exit(0);
            default:
                std::cout << "맞는 번호를 입력하세요." << std::endl;
            }
        }
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "Error Connecting to MariaDB Platform: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}