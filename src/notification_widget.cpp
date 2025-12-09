#include <notification_widget.hpp>

#include <QApplication>
#include <QScreen>
#include <QRegularExpressionMatchIterator>
#include <QFontDatabase>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>

// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppMemberFunctionMayBeConst

// Singleton constructor. Initialize once.
NotificationWidget::NotificationWidget() {
  // Init root overlay. This is an invisible intermediate fullscreen overlay that allows us to open the banner at an arbitary position with support for e.g. background transparency.
  this->overlay_ = new QWidget(nullptr, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus | Qt::WindowStaysOnTopHint);
  this->overlay_->setAttribute(Qt::WA_TranslucentBackground);
  // this->overlay_->setObjectName("root_overlay"); // Pointless?

  // Apply geometries
  this->apply_geometries();

  // Apply stylesheets
  this->apply_stylesheet();

#ifdef _WIN32
  const HWND hwnd = reinterpret_cast<HWND>(this->overlay_->winId());
  LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
  exStyle |= WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW;
  SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
#else
  // TODO: Add Linux support.
#endif
}

// Opens the banner. This needs to apply the icon, title, and description, as they depend on "achievement".
void NotificationWidget::open(const Achievement& achievement) {
  QWidget* banner = new QWidget(this->overlay_);
  banner->setObjectName("root");
  banner->setGeometry(this->banner_geometry_);

  if (this->theme_.icon_enabled) {
    QLabel* icon_widget = new QLabel(banner);
    icon_widget->setObjectName("icon");
    icon_widget->setScaledContents(true);
    const std::string icon_path = (DATA_PATH / "cache" / "icons" / std::to_string(achievement.app_id()) / achievement.icon_name()).generic_string();
    QPixmap pixmap(this->theme_.icon_width, this->theme_.icon_height);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(0, 0, this->theme_.icon_width, this->theme_.icon_height, this->theme_.icon_rounding, this->theme_.icon_rounding);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, this->theme_.icon_width, this->theme_.icon_height, QPixmap(QString::fromStdString(icon_path)));
    painter.end();
    icon_widget->setPixmap(pixmap);
    int icon_x = 0, icon_y = 0;
    if (lntc_cpp::str::equals_ci(this->theme_.icon_anchor, "topleft")) {
      icon_x = this->theme_.icon_offset_x;
      icon_y = this->theme_.icon_offset_y;
    } else if (lntc_cpp::str::equals_ci(this->theme_.icon_anchor, "topright")) {
      icon_x = this->theme_.root_width - this->theme_.icon_width + this->theme_.icon_offset_x;
      icon_y = this->theme_.icon_offset_y;
    } else if (lntc_cpp::str::equals_ci(this->theme_.icon_anchor, "bottomleft")) {
      icon_x = this->theme_.icon_offset_x;
      icon_y = this->theme_.root_height - this->theme_.icon_height + this->theme_.icon_offset_y;
    } else {
      icon_x = this->theme_.root_width - this->theme_.icon_width + this->theme_.icon_offset_x;
      icon_y = this->theme_.root_height - this->theme_.icon_height + this->theme_.icon_offset_y;
    }
    icon_widget->setGeometry(icon_x, icon_y, this->theme_.icon_width, this->theme_.icon_height);
  }

  // Create title
  if (this->theme_.title_enabled) {
    QLabel* title_widget = new QLabel(banner);
    title_widget->setObjectName("title");
    title_widget->setWordWrap(this->theme_.title_font_wrapping);
    title_widget->setText(QString::fromStdString(achievement.names()["english"]));
    QFont font = load_font(this->theme_.title_font, this->theme_.title_font_size);
    font.setBold(this->theme_.title_font_bold);
    font.setItalic(this->theme_.title_font_italic);
    title_widget->setFont(font);
    int title_x = 0, title_y = 0;
    if (lntc_cpp::str::equals_ci(this->theme_.title_anchor, "topleft")) {
      title_x = this->theme_.title_offset_x;
      title_y = this->theme_.title_offset_y;
    } else if (lntc_cpp::str::equals_ci(this->theme_.title_anchor, "topright")) {
      title_x = this->theme_.root_width - title_widget->fontMetrics().horizontalAdvance(QString::fromStdString(achievement.names()["english"])) + this->theme_.title_offset_x;
      title_y = this->theme_.title_offset_y;
    } else if (lntc_cpp::str::equals_ci(this->theme_.title_anchor, "bottomleft")) {
      title_x = this->theme_.title_offset_x;
      title_y = this->theme_.root_height - title_widget->fontMetrics().height() + this->theme_.title_offset_y;
    } else {
      title_x = this->theme_.root_width - title_widget->fontMetrics().horizontalAdvance(QString::fromStdString(achievement.names()["english"])) + this->theme_.title_offset_x;
      title_y = this->theme_.root_height - title_widget->fontMetrics().height() + this->theme_.title_offset_y;
    }
    const int max_width = this->theme_.root_width - title_x - 8;
    title_widget->setFixedWidth(max_width);
    title_widget->move(title_x, title_y);
    title_widget->adjustSize();
  }

  // Create description
  if (this->theme_.description_enabled) {
    QLabel* description_widget = new QLabel(banner);
    description_widget->setObjectName("description");
    description_widget->setWordWrap(this->theme_.description_font_wrapping);
    description_widget->setText(QString::fromStdString(achievement.descriptions()["english"]));
    QFont font = load_font(this->theme_.description_font, this->theme_.description_font_size);
    font.setBold(this->theme_.description_font_bold);
    font.setItalic(this->theme_.description_font_italic);
    description_widget->setFont(font);
    int desc_x = 0, desc_y = 0;
    if (lntc_cpp::str::equals_ci(this->theme_.description_anchor, "topleft")) {
      desc_x = this->theme_.description_offset_x;
      desc_y = this->theme_.description_offset_y;
    } else if (lntc_cpp::str::equals_ci(this->theme_.description_anchor, "topright")) {
      desc_x = this->theme_.root_width - description_widget->fontMetrics().horizontalAdvance(QString::fromStdString(achievement.descriptions()["english"])) + this->theme_.description_offset_x;
      desc_y = this->theme_.description_offset_y;
    } else if (lntc_cpp::str::equals_ci(this->theme_.description_anchor, "bottomleft")) {
      desc_x = this->theme_.description_offset_x;
      desc_y = this->theme_.root_height - description_widget->fontMetrics().height() + this->theme_.description_offset_y;
    } else {
      desc_x = this->theme_.root_width - description_widget->fontMetrics().horizontalAdvance(QString::fromStdString(achievement.descriptions()["english"])) + this->theme_.description_offset_x;
      desc_y = this->theme_.root_height - description_widget->fontMetrics().height() + this->theme_.description_offset_y;
    }
    const int max_width = this->theme_.root_width - desc_x - 8;
    description_widget->setFixedWidth(max_width);
    description_widget->move(desc_x, desc_y);
    description_widget->adjustSize();
  }
  this->overlay_->show();
}

void NotificationWidget::close() {
  this->overlay_->close();
  // ReSharper disable once CppLocalVariableMayBeConst - const members cannot be deleted.
  for (QObject* child : this->overlay_->children()) delete child;
}

// Loads a font from "font" either by path, or by name.
QFont NotificationWidget::load_font(const std::string& font, const int size) {
  if (const std::filesystem::path font_path = ThemeManager::get_singleton()->active_theme_dir() / font; std::filesystem::is_regular_file(font_path)) {
    if (const int id = QFontDatabase::addApplicationFont(QString::fromStdString(font_path.generic_string())); id != -1) {
      const QString family = QFontDatabase::applicationFontFamilies(id).at(0);
      return QFont(family, size);
    }
  }
  return QFont(QString::fromStdString(font), size);
}

// Should be done once at creation, because it takes a lot of time, and stays the same.
void NotificationWidget::apply_stylesheet() {
  QMap<std::string, QMap<std::string, std::string>> stylesheet_map;
  stylesheet_map["#root"]["background"] = this->theme_.root_background;
  stylesheet_map["#root"]["border-radius"] = std::format("{}px", this->theme_.root_rounding);
  stylesheet_map["#icon"]["background"] = "transparent";
  stylesheet_map["#icon"]["border-radius"] = std::format("{}px", this->theme_.icon_rounding);
  stylesheet_map["#title"]["background"] = "transparent";
  stylesheet_map["#title"]["color"] = this->theme_.title_font_color;
  stylesheet_map["#description"]["background"] = "transparent";
  stylesheet_map["#description"]["color"] = this->theme_.description_font_color;

  QRegularExpression re(R"((#[\w]+)\s*\{\s*([^}]+)\s*\})");
  QRegularExpressionMatchIterator i = re.globalMatch(QString::fromStdString(this->theme_.misc_raw_qss));
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    std::string selector = match.captured(1).toStdString();
    std::string body = match.captured(2).toStdString();
    std::istringstream ss(body);
    std::string prop;
    while (std::getline(ss, prop, ';')) {
      if (prop.empty()) continue;
      if (const size_t pos = prop.find(':'); pos != std::string::npos) {
        std::string key = prop.substr(0, pos);
        std::string value = prop.substr(pos + 1);
        key.erase(0, key.find_first_not_of(" \t\n\r"));
        key.erase(key.find_last_not_of(" \t\n\r") + 1);
        value.erase(0, value.find_first_not_of(" \t\n\r"));
        value.erase(value.find_last_not_of(" \t\n\r") + 1);
        stylesheet_map[selector][key] = value;
      }
    }
  }
  QString stylesheet;
  for (auto it = stylesheet_map.constBegin(); it != stylesheet_map.constEnd(); ++it) {
    stylesheet.append(QString::fromStdString(it.key())).append("{");
    const auto& props = it.value();
    for (auto it1 = props.constBegin(); it1 != props.constEnd(); ++it1) {
      const QString key = QString::fromStdString(it1.key());
      const QString value = QString::fromStdString(it1.value());
      if (!value.isEmpty()) stylesheet.append(key).append(":").append(value).append(";");
    }
    stylesheet.append("}");
  }
  log_debug("Stylesheet: {}", stylesheet.toStdString());
  this->overlay_->setStyleSheet(stylesheet);
}

// Should also be done once.
void NotificationWidget::apply_geometries() {
  const QList<QScreen*> screens = QApplication::screens();
  if (screens.empty()) return;

  uint8_t index = FileConfiguration::get_singleton()->get<uint8_t>("display", 0);
  if (index >= screens.size()) index = 0;

  const QScreen* screen = screens[index];
  const QRect screen_geometry = FileConfiguration::get_singleton()->get<bool>("use_full_geometry", false) ? screen->geometry() : screen->availableGeometry();

  overlay_->setGeometry(screen_geometry);

  int x = 0, y = 0;
  if (lntc_cpp::str::equals_ci(theme_.root_anchor, "topleft")) {
    x = theme_.root_offset_x;
    y = theme_.root_offset_y;
  } else if (lntc_cpp::str::equals_ci(theme_.root_anchor, "topright")) {
    x = screen_geometry.width() - theme_.root_width + theme_.root_offset_x;
    y = theme_.root_offset_y;
  } else if (lntc_cpp::str::equals_ci(theme_.root_anchor, "bottomleft")) {
    x = theme_.root_offset_x;
    y = screen_geometry.height() - theme_.root_height + theme_.root_offset_y;
  } else { // bottomright
    x = screen_geometry.width() - theme_.root_width + theme_.root_offset_x;
    y = screen_geometry.height() - theme_.root_height + theme_.root_offset_y;
  }
  this->banner_geometry_ = QRect(QPoint(x, y), QSize(theme_.root_width, theme_.root_height));
}