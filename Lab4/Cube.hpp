#ifndef CUBE_HPP
#define CUBE_HPP
#include <iostream>
#include "Position.hpp"
#include "Constants.hpp"
#include "Colour.hpp"
#include "Cubie.hpp"
#include "RubiksCubeHelper.hpp"

// Класс, описывающий состояние кубика Рубика
class RubiksCube {
public:
	RubiksCube();
	RubiksCube(CenterCubie const&, std::vector<CornerCubie> const&, std::vector<EdgeCubie> const&);
	explicit RubiksCube(std::string const&);
	// Перечисление для определения типа ввода из файла
	enum class FileInputType : bool {
		SWEEP,
		SCRAMBLE
	};
	void read_state(std::string const&, FileInputType const& = FileInputType::SWEEP);
	std::string save_state(std::string = std::string()) const;
	RubiksCube(RubiksCube const&);
	void swap(RubiksCube&);
	RubiksCube(RubiksCube&&) noexcept;
	RubiksCube& operator=(RubiksCube const&);
	RubiksCube& operator=(RubiksCube&&) noexcept;
	~RubiksCube() = default;
	void setup(MoveSequence const&);

	void rotate(Move const&);
	std::string sweep() const;
	Position::Positions face(RubiksCubeHelper::CentersIndexing) const;
	std::vector<Colour> face_colours() const;
	CornerPosition const& corner(RubiksCubeHelper::CornersIndexing) const;
	std::vector<Colour> corner_colours(RubiksCubeHelper::CornersIndexing) const;
	EdgePosition const& edge(RubiksCubeHelper::EdgesIndexing) const;
	std::vector<Colour> edge_colours(RubiksCubeHelper::EdgesIndexing) const;

private:
	CenterCubie center_;
	std::vector<CornerCubie> corners_;
	std::vector<EdgeCubie> edges_;

	void default_init();
	void validate();
	void perform_rotation(RubiksCubeHelper::CornersIndexing, RubiksCubeHelper::CornersIndexing, \
		RubiksCubeHelper::CornersIndexing, RubiksCubeHelper::CornersIndexing d, Move const&);
	void perform_rotation(RubiksCubeHelper::EdgesIndexing, RubiksCubeHelper::EdgesIndexing, \
		RubiksCubeHelper::EdgesIndexing, RubiksCubeHelper::EdgesIndexing, Move const&);
	void perform_rotation(RubiksCubeHelper::CentersIndexing, RubiksCubeHelper::CentersIndexing, \
		RubiksCubeHelper::CentersIndexing, RubiksCubeHelper::CentersIndexing, Move const&);
};

std::ostream& operator<<(std::ostream&, RubiksCube const&);

std::istream& operator>>(std::istream&, RubiksCube&);

RubiksCube random_cube();

#endif
