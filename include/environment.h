#pragma once

#include "storyfilter.h"
#include "libs/UniversalModels/include/TableDataInterface.h"
#include "libs/UniversalModels/include/TableDataListHolder.h"
#include "libs/UniversalModels/include/AdaptingTableModel.h"
#include "include/tasks/fandom_task_processor.h"
#include "include/tasks/author_task_processor.h"
#include "qml_ficmodel.h"
#include "include/core/section.h"
#include "include/pagetask.h"
#include "include/rng.h"
#include "include/tasks/fandom_task_processor.h"
#include "include/pagegetter.h"
#include "querybuilder.h"

namespace interfaces{
class Fandoms;
class Fanfics;
class Authors;
class Tags;
class Genres;
class PageTask;
class RecommendationLists;
}

class FicSource;

class ClientInterface
{
public:

};

class CoreEnvironment : public QObject
{
Q_OBJECT
public:
    CoreEnvironment(QObject* obj = nullptr);

    struct Interfaces{
        // the interface classes used to avoid direct database access in the application
        QSharedPointer<interfaces::Fandoms> fandoms;
        QSharedPointer<interfaces::Fanfics> fanfics;
        QSharedPointer<interfaces::Authors> authors;
        QSharedPointer<interfaces::Tags> tags;
        QSharedPointer<interfaces::Genres> genres;
        QSharedPointer<interfaces::PageTask> pageTask;
        QSharedPointer<interfaces::RecommendationLists> recs;
        QSharedPointer<database::IDBWrapper> db;
        QSharedPointer<database::IDBWrapper> userDb;
        QSharedPointer<database::IDBWrapper> pageCache;
        QSharedPointer<database::IDBWrapper> tasks;
    };


    // reads settings into a files
    void ReadSettings();
    // writes settings into a files
    void WriteSettings();

    void Init();
    static void InitMetatypes();
    // used to set up connections between database and interfaces
    // and between differnt interfaces themselves
    void InitInterfaces();
    void LoadData();
    int GetResultCount();

    void LoadMoreAuthors(QString listname, ECacheMode cacheMode);
    void UseAuthorTask(PageTaskPtr task);
    void UseFandomTask(PageTaskPtr task);
    PageTaskPtr ProcessFandomsAsTask(QList<core::FandomPtr> fandoms,
                                     QString taskComment,
                                     bool allowCacheRefresh,
                                     ECacheMode cacheMode,
                                     QString cutoffText,
                                     ForcedFandomUpdateDate forcedDate = ForcedFandomUpdateDate());


    void UpdateAllAuthorsWith(std::function<void(QSharedPointer<core::Author>, WebPage)> updater);
    // used to fix author names when one of the parsers gets a bunch wrong
    void ReprocessAuthorNamesFromTheirPages();

    // used to create recommendation list from lists/source.txt
    void ProcessListIntoRecommendations(QString list);

    int  BuildRecommendationsServerFetch(QSharedPointer<core::RecommendationList> params);
    int  BuildRecommendationsLocalVersion(QSharedPointer<core::RecommendationList> params, bool clearAuthors = true);
    int  BuildRecommendations(QSharedPointer<core::RecommendationList> params, bool clearAuthors = true);

    void ResumeUnfinishedTasks();

    void CreateSimilarListForGivenFic(int id,  QSqlDatabase db);

    void Log(QString);

    core::AuthorPtr LoadAuthor(QString url, QSqlDatabase db);

    PageTaskPtr LoadTrackedFandoms(ForcedFandomUpdateDate forcedDate, ECacheMode cacheMode, QString wordCutoff);
    void FillDBIDsForTags();

    core::StoryFilter filter; // an intermediary to keep UI filter data to be passed into query builder
    Interfaces interfaces;

    int sizeOfCurrentQuery = 0; // "would be" size of the used search query if LIMIT  was not applied
    int pageOfCurrentQuery = 0; // current page that the used search query is at
    int currentLastFanficId = -1;


    QVector<core::Fic> fanfics; // filtered fanfic data

    QSharedPointer<core::Query> currentQuery; // the last query created by query builder. reused when querying subsequent pages

    QSharedPointer<FicSource> ficSource;
    QSharedPointer<core::IRNGGenerator> rngGenerator;
    QString userToken;


    // in case of non-gui applications these will just fire without an effect and its correct
signals:
    void requestProgressbar(int);
    void resetEditorText();
    void updateCounter(int);
    void updateInfo(QString);
};
namespace env {
    WebPage RequestPage(QString pageUrl, ECacheMode forcedCacheMode = ECacheMode::use_cache, bool autoSaveToDB = false);
}

