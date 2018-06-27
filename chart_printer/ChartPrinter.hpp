#ifndef CHART_PRINTER_HPP
#define CHART_PRINTER_HPP

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>



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
	sf::Color color;
	sf::Text text;

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





class ChartPrinter: public sf::Drawable, public sf::Transformable
{
public:
	typedef float value_type;
	typedef std::vector<
		std::pair<value_type, value_type>
	> chart_type;
	typedef std::shared_ptr<chart_type> chartptr_type;




	void update();

	void draw(
		sf::RenderTarget &target,
		sf::RenderStates states = sf::RenderStates::Default
	) const override;


	ChartPrinter &addChart(
		chartptr_type newchart,
		ChartSettings const &settings =
			ChartSettings::getDefault()
	);
	ChartPrinter &removeChart(chartptr_type const &chart);
	ChartPrinter &clearCharts();



	ChartPrinter &setAxisSettings(
		AxisSettings const &newaxis
	);
	AxisSettings const &getAxisSettings() const;


	ChartPrinter &setTagSettings(
		TagSettings const &newtagset
	);
	TagSettings const &getTagSettings() const;

	ChartPrinter &setGridSettings(
		GridSettings const &gridset
	);
	GridSettings getGridSettings() const;

	void calculate_tags_bycount(size_t xcount = 10, size_t ycount = 10);

	void calculate_tags_byinterval(float xinter, float yinter);



	ChartPrinter &setSize(float width, float height);
	ChartPrinter &setSize(sf::Vector2f size);

	sf::Vector2f getSize() const;

	ChartPrinter &setPadding(float padding);
	float getPadding() const;


private:
	void adjust_();

	void calculate_size_();

	void draw_();
	void draw_axis_();
	void draw_tags_();
	void draw_grid_();
	void draw_charts_();



	float width_ = 300.0f, height_ = 300.0f;
	float padding_ = 0.0f;
	sf::Vector2f axispoint_ = {0.0f, 0.0f};
	AxisSettings axis_ = AxisSettings::getDefault();

	std::vector<
		std::pair<chartptr_type, ChartSettings>
	> charts_;
	float xl_ = 0.0f, yl_ = 0.0f;
	float xmin_ = 0.0f, ymin_ = 0.0f;

	Tags tags_;
	TagSettings tagset_ = TagSettings::getDefault();
	GridSettings gridset_ = GridSettings::getDefault();



	bool ischanged_ = true;

	sf::RenderTexture rtexture_;
	sf::Sprite sprite_;

};





#endif
