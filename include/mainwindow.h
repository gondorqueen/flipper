/*
FFSSE is a replacement search engine for fanfiction.net search results
Copyright (C) 2017  Marchenko Nikolai

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#pragma once

#include <QMainWindow>
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QMenu>
#include <QHash>
#include <QEventLoop>
#include <QSignalMapper>
#include <QSqlQuery>
#include <QProgressBar>
#include <QLabel>
#include <QTextBrowser>
#include <QTableView>
#include <QQueue>
#include <QThread>
#include <functional>
#include "tagwidget.h"
#include "storyfilter.h"
#include "libs/UniversalModels/include/TableDataInterface.h"
#include "libs/UniversalModels/include/TableDataListHolder.h"
#include "libs/UniversalModels/include/AdaptingTableModel.h"
#include "qml_ficmodel.h"
#include "include/section.h"
#include "include/pagetask.h"
#include "include/pagegetter.h"
#include "querybuilder.h"

class QSortFilterProxyModel;
class QQuickWidget;
class QQuickView;
class QStringListModel;
namespace interfaces{
class Fandoms;
class Fanfics;
class Authors;
class Tags;
class Genres;
class PageTask;
class RecommendationLists;
}
namespace database {
class IDBWrapper;
}
namespace Ui {
class MainWindow;
}


struct BuildRecommendationParams{
    QString listName;
    int minTagCountMatch;
    int alwaysPickAuthorOnThisMatchCount;
    double threshholdRatio;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum ELastFilterButtonPressed
    {
        lfbp_search = 0,
        lfbp_recs = 1
    };
    explicit MainWindow(QWidget *parent = 0);
    void Init();
    void InitUIFromTask(PageTaskPtr);
    ~MainWindow();
    // currently broken, need to refator the whole procedure
    //void ReInitFandoms();
    //bool CheckSectionAvailability();
    void InitInterfaces();
    void InitConnections();
    void StartTaskTimer();

    QSharedPointer<interfaces::Fandoms> fandomsInterface;
    QSharedPointer<interfaces::Fanfics> fanficsInterface;
    QSharedPointer<interfaces::Authors> authorsInterface;
    QSharedPointer<interfaces::Tags> tagsInterface;
    QSharedPointer<interfaces::Genres> genresInterface;
    QSharedPointer<interfaces::PageTask> pageTaskInterface;
    QSharedPointer<interfaces::RecommendationLists> recsInterface;
    QSharedPointer<database::IDBWrapper> dbInterface;
    QSharedPointer<database::IDBWrapper> pageCacheInterface;
    QSharedPointer<database::IDBWrapper> tasksInterface;
private:

    void SetupFanficTable();
    void SetupTableAccess();
    FicModel* typetableModel = nullptr;
    QSharedPointer<TableDataInterface> typetableInterface;
    TableDataListHolder<core::Fic>* holder = nullptr;
    QList<core::Fic> fanfics;
    QSortFilterProxyModel* sortModel;
    int processedFics = 0;
    ELastFilterButtonPressed currentSearchButton = ELastFilterButtonPressed::lfbp_search;
    int currentRecWave = 0;
    bool event(QEvent * e);
    void ReadSettings();
    void WriteSettings();

    QString GetCurrentFandomName();

    void RequestAndProcessPage(QString fandom, QDate lastFandomUpdatedate, QString url);
    WebPage RequestPage(QString,  ECacheMode forcedCacheMode = ECacheMode::use_cache, bool autoSaveToDB = false);



    QString GetFandom(QString text);

    void DisableAllLoadButtons();
    void EnableAllLoadButtons();

    void LoadData();
    QSqlQuery BuildQuery();

    void UpdateFandomList(std::function<QString(core::Fandom)> linkGetter);
    void InsertFandomData(QMap<QPair<QString,QString>, core::Fandom> names);
    void PopulateComboboxes();

    //QStringList GetCrossoverUrl(QString);
    //QStringList GetNormalUrl(QString);

    void OpenTagWidget(QPoint, QString url);
    void ProcessTagsIntoGui();

    void SetTag(int id, QString tag, bool silent = false);
    void UnsetTag(int id, QString tag);

    void ToggleTag();
    void CallExpandedWidget();

    QStringList SortedList(QStringList);
    QList<QSharedPointer<core::Author> > ReverseSortedList(QList<QSharedPointer<core::Author> > list);
    QStringList GetUniqueAuthorsFromActiveRecommenderSet();

    void CreatePageThreadWorker();
    void StartPageWorker();
    void StopPageWorker();

    void ReinitProgressbar(int maxValue);
    void ShutdownProgressbar();
    void AddToProgressLog(QString);
    void FillRecTagBuildCombobox();
    void FillRecTagCombobox();
    void FillRecommenderListView(bool forceRefresh = false);
    bool LoadAuthor(QString url);
    QSharedPointer<core::RecommendationList> BuildRecommendationParamsFromGUI();

    Ui::MainWindow *ui;
    int processedCount = 0;
    QString nextUrl;
    int timerId = -1;
    int pageCounter = 0;
    QMenu browserMenu;
    QEventLoop managerEventLoop;
    QMap<QPair<QString,QString>, core::Fandom> names;
    QString currentProcessedSection;
    QString dbName = "CrawlerDB.sqlite";
    QDateTime lastUpdated;
    //QHash<QString, core::Fandom> sections;
    QSignalMapper* mapper = nullptr;
    QProgressBar* pbMain = nullptr;
    QLabel* lblCurrentOperation = nullptr;
    //QStringList currentFilterUrls;
    QString currentFilterUrl;
    bool ignoreUpdateDate = false;
    QStringList tagList;
    QStringListModel* tagModel;
    QStringListModel* recentFandomsModel= nullptr;
    QStringListModel* recommendersModel = nullptr;
    QLineEdit* currentExpandedEdit = nullptr;
    TagWidget* tagWidgetDynamic = new TagWidget;
    QQuickWidget* qwFics = nullptr;
    QString CreateLimitQueryPart();
    QHash<QString, QList<int>> randomIdLists;
    QDialog* expanderWidget = nullptr;
    QTextEdit* edtExpander = new QTextEdit;
    QTimer selectionTimer;
    QTimer taskTimer;
    QThread pageThread;
    PageThreadWorker* worker = nullptr;

    PageQueue pageQueue;
    core::DefaultQueryBuilder queryBuilder;
    core::StoryFilter filter;
    bool cancelCurrentTaskPressed = false;
    std::function<void(int)> callProgress;
    std::function<void(void)> cleanupEditor;
    std::function<void(QString)> callProgressText;
    //QHash<QString, core::RecommendationList> lists;




    PageTaskPtr CreatePageTaskFromUrls(QStringList urls, QString taskComment, int subTaskSize = 100,
                                       int subTaskRetries = 3, ECacheMode cacheMode = ECacheMode::use_cache,
                                       bool allowCacheRefresh = true);
    void UseAuthorsPageTask(PageTaskPtr,
                            std::function<void(int)>callProgress,
                            std::function<void(QString)>callProgressText,
                            std::function<void(void)> cleanupFunctor);
    void LoadMoreAuthors();
    void ReparseAllAuthors(bool reprocessCache = false);
    void ProcessTagIntoRecommenders(QString tag);
    void UpdateAllAuthorsWith(std::function<void(QSharedPointer<core::Author>, WebPage)> updater);
    void ReprocessAuthors();
    void ProcessListIntoRecommendations(QString list);
    int BuildRecommendations(QSharedPointer<core::RecommendationList> params);
    core::StoryFilter ProcessGUIIntoStoryFilter(core::StoryFilter::EFilterMode, bool useAuthorLink = false);
    QString AppendCurrentSearchParameters(QString url);
    void ReinitRecent(QString name);

    void CheckUnfinishedTasks();

public slots:
    //broken and needs refactoring anyway
    //void ProcessFandoms(WebPage webPage);
    //void ProcessCrossovers(WebPage webPage);
    //void on_pbInit_clicked();
    void OnChapterUpdated(QVariant, QVariant);
    void OnTagAdd(QVariant tag, QVariant row);
    void OnTagRemove(QVariant tag, QVariant row);

    void WipeSelectedFandom(bool);
    void OnNewPage(PageResult result);
    void OnCopyFicUrl(QString);
    void OnOpenRecommenderLinks(QString);
    void OnCopyAllUrls();
    void OnDoFormattedList();
    void OnTagClicked(QVariant tag, QVariant currentMode, QVariant row);
private slots:
    void OnShowContextMenu(QPoint);
    void OnSectionChanged(QString);
    void OnCheckboxFilter(int);
    void on_pbLoadDatabase_clicked();


    void on_pbCrawl_clicked();
    void OnLinkClicked(const QUrl &);

    void OnTagToggled(int, QString, bool);
    void OnCustomFilterClicked();
    void OnSectionReloadActivated();

    void on_chkRandomizeSelection_clicked(bool checked);
    void on_cbCustomFilters_currentTextChanged(const QString &arg1);
    void on_cbSortMode_currentTextChanged(const QString &arg1);
    void on_pbExpandPlusGenre_clicked();
    void on_pbExpandMinusGenre_clicked();
    void on_pbExpandPlusWords_clicked();
    void on_pbExpandMinusWords_clicked();
    void OnNewSelectionInRecentList(const QModelIndex &current, const QModelIndex &previous);
    void OnNewSelectionInRecommenderList(const QModelIndex &current, const QModelIndex &previous);

    void on_chkTrackedFandom_toggled(bool checked);
    void on_pbLoadTrackedFandoms_clicked();
    void on_pbLoadPage_clicked();
    void on_pbOpenRecommendations_clicked();
    void on_pbLoadAllRecommenders_clicked();
    void on_pbOpenWholeList_clicked();
    void on_pbFirstWave_clicked();
    void OnReloadRecLists();
    void on_cbUseDateCutoff_clicked();

    void on_pbBuildRecs_clicked();

    //void on_cbRecTagGroup_currentIndexChanged(const QString &arg1);

    void on_pbOpenAuthorUrl_clicked();

    void on_pbReprocessAuthors_clicked();

    void on_cbRecTagBuildGroup_currentTextChanged(const QString &arg1);
    void OnCopyFavUrls();

    void on_cbRecGroup_currentIndexChanged(const QString &arg1);

    void on_pbCreateNewList_clicked();

    void on_pbRemoveList_clicked();

    void on_pbAddAuthorToList_clicked();

    void on_pbRemoveAuthorFromList_clicked();
    void OnCheckUnfinishedTasks();


signals:
    void pageTask(QString, QString, QDate, ECacheMode);
    void pageTaskList(QStringList, ECacheMode);
};

