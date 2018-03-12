/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2018 Viet Dang (dangvd@gmail.com)
 *
 * KSmoothDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * KSmoothDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KSmoothDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KSMOOTHDOCK_MULTI_DOCK_MODEL_H_
#define KSMOOTHDOCK_MULTI_DOCK_MODEL_H_

#include <memory>
#include <unordered_map>
#include <utility>

#include <QColor>
#include <QDir>
#include <QObject>
#include <QString>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

#include "config_helper.h"

namespace ksmoothdock {

constexpr int kDefaultMinSize = 48;
constexpr int kDefaultMaxSize = 128;
constexpr int kDefaultTooltipFontSize = 20;
constexpr float kDefaultBackgroundAlpha = 0.42;
constexpr char kDefaultBackgroundColor[] = "#638abd";
constexpr bool kDefaultShowBorder = true;
constexpr char kDefaultBorderColor[] = "#b1c4de";
constexpr float kLargeClockFontScaleFactor = 1.0;
constexpr float kMediumClockFontScaleFactor = 0.8;
constexpr float kSmallClockFontScaleFactor = 0.6;

constexpr bool kDefaultAutoHide = false;
constexpr bool kDefaultShowApplicationMenu = true;
constexpr bool kDefaultShowPager = false;
constexpr bool kDefaultShowClock = false;

constexpr char kDefaultApplicationMenuName[] = "Applications";
constexpr char kDefaultApplicationMenuIcon[] = "start-here-kde";
constexpr bool kDefaultUse24HourClock = true;
constexpr float kDefaultClockFontScaleFactor = kLargeClockFontScaleFactor;

enum class PanelPosition {Top, Bottom, Left, Right};

struct LauncherConfig {
  QString name;
  QString icon;
  QString command;

  LauncherConfig() = default;
  LauncherConfig(const QString& name2, const QString& icon2,
                 const QString& command2)
      : name(name2), icon(icon2), command(command2) {}
  LauncherConfig(const QString& desktopFile);

  // Saves to file in desktop file format.
  void saveToFile(const QString& filePath) const;
};

// The model.
class MultiDockModel : public QObject {
  Q_OBJECT

 public:
  MultiDockModel(const QString& configDir);
  ~MultiDockModel() = default;

  MultiDockModel(const MultiDockModel&) = delete;
  MultiDockModel& operator=(const MultiDockModel&) = delete;

  // Returns the number of docks.
  int dockCount() const { return dockConfigs_.size(); }

  // Adds a new dock in the specified position and screen.
  void addDock(PanelPosition position, int screen);

  // Clones an existing dock in the specified position and screen.
  void cloneDock(int srcDockId, PanelPosition position, int screen);

  // Removes a dock.
  void removeDock(int dockId);

  int minIconSize() const {
    return appearanceProperty(kGeneralCategory, kMinimumIconSize,
                              kDefaultMinSize);
  }

  void setMinIconSize(int value) {
    setAppearanceProperty(kGeneralCategory, kMinimumIconSize, value);
  }

  int maxIconSize() const {
    return appearanceProperty(kGeneralCategory, kMaximumIconSize,
                              kDefaultMaxSize);
  }

  void setMaxIconSize(int value) {
    setAppearanceProperty(kGeneralCategory, kMaximumIconSize, value);
  }

  QColor backgroundColor() const {
    QColor defaultBackgroundColor(kDefaultBackgroundColor);
    defaultBackgroundColor.setAlphaF(kDefaultBackgroundAlpha);
    return appearanceProperty(kGeneralCategory, kBackgroundColor,
                              defaultBackgroundColor);
  }

  void setBackgroundColor(const QColor& value) {
    setAppearanceProperty(kGeneralCategory, kBackgroundColor, value);
  }

  bool showBorder() const {
    return appearanceProperty(kGeneralCategory, kShowBorder,
                              kDefaultShowBorder);
  }

  void setShowBorder(bool value) {
    setAppearanceProperty(kGeneralCategory, kShowBorder, value);
  }

  QColor borderColor() const {
    return appearanceProperty(kGeneralCategory, kBorderColor,
                              QColor(kDefaultBorderColor));
  }

  void setBorderColor(const QColor& value) {
    setAppearanceProperty(kGeneralCategory, kBorderColor, value);
  }

  int tooltipFontSize() const {
    return appearanceProperty(kGeneralCategory, kTooltipFontSize,
                              kDefaultTooltipFontSize);
  }

  void setTooltipFontSize(int value) {
    setAppearanceProperty(kGeneralCategory, kTooltipFontSize, value);
  }

  QString applicationMenuName() const {
    return appearanceProperty(kApplicationMenuCategory, kLabel,
                              i18n(kDefaultApplicationMenuName));
  }

  void setApplicationMenuName(const QString& value) {
    setAppearanceProperty(kApplicationMenuCategory, kLabel, value);
  }

  QString applicationMenuIcon() const {
    return appearanceProperty(kApplicationMenuCategory, kIcon,
                              QString(kDefaultApplicationMenuIcon));
  }

  void setApplicationMenuIcon(const QString& value) {
    setAppearanceProperty(kApplicationMenuCategory, kIcon, value);
  }

  QString wallpaper(int desktop) const {
    return appearanceProperty(kPagerCategory,
                              ConfigHelper::wallpaperConfigKey(desktop),
                              QString());
  }

  void setWallpaper(int desktop, const QString& value) {
    setAppearanceProperty(kPagerCategory,
                          ConfigHelper::wallpaperConfigKey(desktop), value);
  }

  bool use24HourClock() const {
    return appearanceProperty(kClockCategory, kUse24HourClock,
                              kDefaultUse24HourClock);
  }

  void setUse24HourClock(bool value) {
    setAppearanceProperty(kClockCategory, kUse24HourClock, value);
  }

  float clockFontScaleFactor() const {
    return appearanceProperty(kClockCategory, kFontScaleFactor,
                              kDefaultClockFontScaleFactor);
  }

  void setClockFontScaleFactor(float value) {
    setAppearanceProperty(kClockCategory, kFontScaleFactor, value);
  }

  void saveAppearanceConfig() {
    syncAppearanceConfig();
    emit appearanceChanged();
  }

  PanelPosition panelPosition(int dockId) const {
    return static_cast<PanelPosition>(dockProperty(
        dockId, kGeneralCategory, kPosition,
        static_cast<int>(PanelPosition::Bottom)));
  }

  void setPanelPosition(int dockId, PanelPosition value) {
    setDockProperty(dockId, kGeneralCategory, kPosition,
                    static_cast<int>(value));
  }

  int screen(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kScreen, 0);
  }

  void setScreen(int dockId, int value) {
    setDockProperty(dockId, kGeneralCategory, kScreen, value);
  }

  bool autoHide(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kAutoHide, kDefaultAutoHide);
  }

  void setAutoHide(int dockId, bool value) {
    setDockProperty(dockId, kGeneralCategory, kAutoHide, value);
  }

  bool showApplicationMenu(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kShowApplicationMenu,
                        kDefaultShowApplicationMenu);
  }

  void setShowApplicationMenu(int dockId, bool value) {
    setDockProperty(dockId, kGeneralCategory, kShowApplicationMenu, value);
  }

  bool showPager(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kShowPager,
                        kDefaultShowPager);
  }

  void setShowPager(int dockId, bool value) {
    setDockProperty(dockId, kGeneralCategory, kShowPager, value);
  }

  bool showClock(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kShowClock,
                        kDefaultShowClock);
  }

  void setShowClock(int dockId, bool value) {
    setDockProperty(dockId, kGeneralCategory, kShowClock, value);
  }

  void saveDockConfig(int dockId) {
    syncDockConfig(dockId);
    // No need to emit signal here.
  }

  QString dockLaunchersPath(int dockId) const {
    return std::get<2>(dockConfigs_.at(dockId));
  }

  const std::vector<LauncherConfig> dockLauncherConfigs(int dockId) const {
    return std::get<3>(dockConfigs_.at(dockId));
  }

  void setDockLauncherConfigs(
      int dockId, const std::vector<LauncherConfig>& launcherConfigs) {
    std::get<3>(dockConfigs_[dockId]) = launcherConfigs;
  }

  void saveDockLauncherConfigs(int dockId) {
    syncDockLaunchersConfig(dockId);
    emit dockLaunchersChanged(dockId);
  }

 signals:
  void appearanceChanged();
  void dockAdded(int dockId);
  void dockLaunchersChanged(int dockId);

 private:
  // Dock config's categories/properties.
  static constexpr char kGeneralCategory[] = "General";
  static constexpr char kAutoHide[] = "autoHide";
  static constexpr char kPosition[] = "position";
  static constexpr char kScreen[] = "screen";
  static constexpr char kShowApplicationMenu[] = "showApplicationMenu";
  static constexpr char kShowClock[] = "showClock";
  static constexpr char kShowPager[] = "showPager";

  // Global appearance config's categories/properties.

  // General category.
  static constexpr char kBackgroundColor[] = "backgroundColor";
  static constexpr char kBorderColor[] = "borderColor";
  static constexpr char kMaximumIconSize[] = "maximumIconSize";
  static constexpr char kMinimumIconSize[] = "minimumIconSize";
  static constexpr char kShowBorder[] = "showBorder";
  static constexpr char kTooltipFontSize[] = "tooltipFontSize";

  static constexpr char kApplicationMenuCategory[] = "Application Menu";
  static constexpr char kIcon[] = "icon";
  static constexpr char kLabel[] = "label";

  static constexpr char kPagerCategory[] = "Pager";
  static constexpr char kWallpaper[] = "wallpaper";

  static constexpr char kClockCategory[] = "Clock";
  static constexpr char kUse24HourClock[] = "use24HourClock";
  static constexpr char kFontScaleFactor[] = "fontScaleFactor";

  template <typename T>
  T appearanceProperty(QString category, QString name, T defaultValue) const {
    KConfigGroup group(&appearanceConfig_, category);
    return group.readEntry(name, defaultValue);
  }

  template <typename T>
  void setAppearanceProperty(QString category, QString name, T value) {
    KConfigGroup group(&appearanceConfig_, category);
    group.writeEntry(name, value);
  }

  template <typename T>
  T dockProperty(int dockId, QString category, QString name, T defaultValue)
      const {
    KConfigGroup group(dockConfig(dockId), category);
    return group.readEntry(name, defaultValue);
  }

  template <typename T>
  void setDockProperty(int dockId, QString category, QString name, T value) {
    KConfigGroup group(dockConfig(dockId), category);
    group.writeEntry(name, value);
  }

  QString dockConfigPath(int dockId) const {
    return std::get<0>(dockConfigs_.at(dockId));
  }

  const KConfig* dockConfig(int dockId) const {
    return std::get<1>(dockConfigs_.at(dockId)).get();
  }

  KConfig* dockConfig(int dockId) {
    return std::get<1>(dockConfigs_[dockId]).get();
  }

  static std::vector<LauncherConfig> loadDockLaunchers(
      const QString& dockLaunchersPath);

  static std::vector<LauncherConfig> createDefaultLaunchers();

  void loadDocks();

  int addDock(const std::tuple<QString, QString>& configs,
              PanelPosition position, int screen);

  void syncAppearanceConfig() {
    appearanceConfig_.sync();
  }

  void syncDockConfig(int dockId) {
    dockConfig(dockId)->sync();
  }

  void syncDockLaunchersConfig(int dockId);

  static void copyEntry(const QString& key, const KConfigGroup& sourceGroup,
                        KConfigGroup* destGroup) {
    destGroup->writeEntry(key, sourceGroup.readEntry(key));
  }

  // Converts the old single-dock config to the new multi-dock config if needed.
  bool convertConfig();

  // Helper(s).
  ConfigHelper configHelper_;

  // Model data.

  // Appearance config.
  KConfig appearanceConfig_;

  // Dock configs, as map from dockIds to tuples of:
  // (dock config file path,
  //  dock config,
  //  launchers dir path,
  //  list of launcher configs)
  std::unordered_map<int,
                     std::tuple<QString,
                                std::unique_ptr<KConfig>,
                                QString,
                                std::vector<LauncherConfig>>> dockConfigs_;

  // ID for the next dock.
  int nextDockId_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_MULTI_DOCK_MODEL_H_
