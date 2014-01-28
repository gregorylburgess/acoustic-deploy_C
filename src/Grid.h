class Grid {
	public:
		int rows, cols;
		double* data;
		Grid();
		Grid(int Rows, int Cols);
		Grid(Grid* mat);
		~Grid();
};
