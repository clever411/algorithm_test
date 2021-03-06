#ifndef CHART_PRINTER_HPP
#define CHART_PRINTER_HPP

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>



// settings structures
struct ChartSettings
{
	sf::Color color;
	float thickness;
	float overlayprior;

	static ChartSettings const &getDefault();
};


struct AxisSettings
{
	sf::Color color;
	float thickness;

	static AxisSettings const &getDefault();
};


struct TagSettings
{
	float length;
	float thickness;

	float xinter;
	float yinter;
	float xyratio;
	sf::Color tcolor;

	float pxinter;
	float pyinter;

	int xlabelfreq;
	int ylabelfreq;
	sf::Text text;
	int fontsize;

	static TagSettings const &getDefault();
};


struct Tags
{
	std::vector<float> abscissa;
	std::vector<float> ordinate;
};


struct GridSettings
{
	bool enable;
	float thickness;
	sf::Color color;

	static GridSettings const &getDefault();
};


struct AimSettings
{
	float thickness;
	sf::Color color;

	static AimSettings const &getDefault();
};


struct TableSettings
{
	sf::Vector2f padding;
	sf::Text text;

	static TableSettings const &getDefault();
};





// chart printer
class ChartPrinter: public sf::Drawable, public sf::Transformable
{
public:
	// types
	typedef float value_type;
	typedef std::vector<
		std::pair<value_type, value_type>
	> chart_type;
	typedef std::shared_ptr<chart_type> chartptr_type;



	// using methods
	void update();

	void draw(
		sf::RenderTarget &target,
		sf::RenderStates states = sf::RenderStates::Default
	) const override;

	void drawAim(
		sf::Vector2f const &pixpoint,
		sf::RenderTarget &target,
		sf::RenderStates states = sf::RenderStates::Default
	) const;

	void drawTable(
		sf::Vector2f pixpoint,
		sf::RenderTarget &target,
		sf::RenderStates states = sf::RenderStates::Default
	) const;




	// size
	ChartPrinter &setSize(float width, float height);
	ChartPrinter &setSize(sf::Vector2f const &size);
	sf::Vector2f getSize() const;


	// padding
	ChartPrinter &setPadding(float padding);
	float getPadding() const;



	// charts
	ChartPrinter &addChart(
		chartptr_type newchart,
		ChartSettings const &settings =
			ChartSettings::getDefault()
	);
	ChartPrinter &removeChart(chartptr_type const &chart);
	ChartPrinter &clearCharts();



	// axis settings
	ChartPrinter &setAxisSettings(
		AxisSettings const &newaxis
	);
	AxisSettings const &getAxisSettings() const;


	// tag settings
	ChartPrinter &setTagSettings(
		TagSettings const &newtagset
	);
	TagSettings const &getTagSettings() const;

	ChartPrinter &generateTagsByCount(size_t xcount = 10, size_t ycount = 10);
	ChartPrinter &generateTagsByInterval(float xinter, float yinter);


	// grid settings
	ChartPrinter &setGridSettings(
		GridSettings const &gridset
	);
	GridSettings const &getGridSettings() const;


	// cross settings
	ChartPrinter &setAimSettings(
		AimSettings const &crset
	);
	AimSettings const &getAimSettings() const;


	// table settings
	ChartPrinter &setTableSettings(
		TableSettings const &crset
	);
	TableSettings const &getTableSettings() const;



	// other
	sf::Vector2f descartesToPixels(sf::Vector2f const &point) const;
	sf::Vector2f pixelsToDescartes(sf::Vector2f const &point) const;
	


private:
	// service methods
	void adjust_();

	void calculate_charts_characts_();
	void calculate_smart_size_();
	void calculate_xkyk_();
	void calculate_font_size_and_frequency_();
	void calculate_axis_point_();
	void generate_tags_();

	void draw_();
	void draw_grid_();
	void draw_axis_();
	void draw_tags_();
	void draw_charts_();

	static float make_beauty_(float n);


	

	// size on pixels
	float width_ = 300.0f, height_ = 300.0f;
	float padding_ = 0.0f;
	sf::Vector2f axispoint_ = {0.0f, 0.0f};


	// charts
	std::vector<
		std::pair<chartptr_type, ChartSettings>
	> charts_;

	//size on descartes
	float xl_ = 0.0f, yl_ = 0.0f;
	float xmin_ = 0.0f, ymin_ = 0.0f;
	float xk_ = 0.0f, yk_ = 0.0f;

	// tags on descartes
	Tags tags_;


	// settings
	AxisSettings axis_ = AxisSettings::getDefault();
	TagSettings tagset_ = TagSettings::getDefault();
	GridSettings gridset_ = GridSettings::getDefault();
	AimSettings crset_ = AimSettings::getDefault();
	mutable TableSettings tabset_ = TableSettings::getDefault();



	// technical
	bool ischanged_ = true;

	sf::RenderTexture rtexture_;
	sf::Sprite sprite_;



};





#endif
