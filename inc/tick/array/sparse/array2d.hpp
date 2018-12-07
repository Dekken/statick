#ifndef TICK_ARRAY_ARRAY_HPP_
#define TICK_ARRAY_ARRAY_HPP_

namespace tick {
template <class Archive, class T>
bool load_sparse2d_with_raw_data(Archive &ar, std::vector<T> &data, std::vector<size_t> &info,
                                 std::vector<INDICE_TYPE> &indices, std::vector<INDICE_TYPE> &row_indices) {
  size_t rows = 0, cols = 0, size_sparse, size = 0;
  ar(size_sparse, rows, cols, size);

  if (data.size() < 3) data.resize(size_sparse);
  if (info.size() < 3) info.resize(3);
  if (indices.size() < size_sparse) indices.resize(size_sparse);
  if (row_indices.size() < (rows + 1)) row_indices.resize(rows + 1);

  ar(cereal::binary_data(data.data(), sizeof(T) * size_sparse));
  ar(cereal::binary_data(indices.data(), sizeof(INDICE_TYPE) * size_sparse));
  ar(cereal::binary_data(row_indices.data(), sizeof(INDICE_TYPE) * (rows + 1)));

  info[0] = cols;
  info[1] = rows;
  info[2] = size_sparse;
  return true;
}

template <class T>
class Sparse2D {
 public:
  Sparse2D() {}
  Sparse2D(Sparse2D &&that)
      : m_data(that.m_data), m_info(that.m_info), m_indices(that.m_indices), m_row_indices(that.m_row_indices) {}

  const T *data() const { return m_data.data(); }
  const T *row_raw(size_t i) const { return m_data.data() + m_row_indices[i]; }
  INDICE_TYPE row_size(size_t i) const { return m_row_indices[i + 1] - m_row_indices[i]; }
  const INDICE_TYPE *indices() const { return m_indices.data(); }
  const INDICE_TYPE *row_indices(size_t i) const { return m_indices.data() + m_row_indices[i]; }
  const INDICE_TYPE *row_indices() const { return m_row_indices.data(); }

  Sparse<T> row(size_t i) const {
    return Sparse<T>(m_data.data() + m_row_indices[i], m_row_indices[i + 1] - m_row_indices[i],
                     m_indices.data() + m_row_indices[i]);
  }

  const size_t &cols() const { return m_info[0]; }
  const size_t &rows() const { return m_info[1]; }
  const size_t &size() const { return m_info[2]; }

  static std::shared_ptr<Sparse2D<T>> FROM_CEREAL(const std::string &file) {
    auto array = std::make_shared<Sparse2D<T>>();
    {
      std::ifstream bin_data(file, std::ios::in | std::ios::binary);
      cereal::PortableBinaryInputArchive iarchive(bin_data);
      if (tick::load_sparse2d_with_raw_data(iarchive, array->m_data, array->m_info, array->m_indices,
                                            array->m_row_indices))
        return std::move(array);
    }
    return nullptr;
  }

 private:
  std::vector<T> m_data;
  std::vector<size_t> m_info;
  std::vector<INDICE_TYPE> m_indices, m_row_indices;

  Sparse2D(Sparse2D &that) = delete;
  Sparse2D(const Sparse2D &that) = delete;
  Sparse2D(const Sparse2D &&that) = delete;
  Sparse2D &operator=(Sparse2D &that) = delete;
  Sparse2D &operator=(Sparse2D &&that) = delete;
  Sparse2D &operator=(const Sparse2D &that) = delete;
  Sparse2D &operator=(const Sparse2D &&that) = delete;
};

template <class T>
class RawSparse2D {
 public:
  RawSparse2D(const T *_data, const size_t *_info, const INDICE_TYPE *_indices, const INDICE_TYPE *_row_indices)
      : v_data(_data),
        m_cols(&_info[0]),
        m_rows(&_info[1]),
        m_size(&_info[2]),
        v_indices(_indices),
        v_row_indices(_row_indices) {}
  RawSparse2D(RawSparse2D &&that)
      : v_data(that.v_data),
        m_cols(that.m_cols),
        m_rows(that.m_rows),
        m_size(that.m_size),
        v_indices(that.v_indices),
        v_row_indices(that.v_row_indices) {}
  T &operator[](int i) { return v_data[i]; }
  const T *data() const { return v_data; }
  const T *row_raw(size_t i) const { return v_data + v_row_indices[i]; }
  INDICE_TYPE row_size(size_t i) const { return v_row_indices[i + 1] - v_row_indices[i]; }
  const INDICE_TYPE *indices() const { return v_indices; }
  const INDICE_TYPE *row_indices(size_t i) const { return v_indices + v_row_indices[i]; }
  const INDICE_TYPE *row_indices() const { return v_row_indices; }

  Sparse<T> row(size_t i) const {
    return Sparse<T>(v_data + v_row_indices[i], v_row_indices[i + 1] - v_row_indices[i], v_indices + v_row_indices[i]);
  }

  const size_t &cols() const { return *m_cols; }
  const size_t &rows() const { return *m_rows; }
  const size_t &size() const { return *m_size; }

 private:
  const T *v_data;
  const size_t *m_cols, *m_rows, *m_size;
  const INDICE_TYPE *v_indices, *v_row_indices;

  RawSparse2D() = delete;
  RawSparse2D(RawSparse2D &that) = delete;
  RawSparse2D(const RawSparse2D &that) = delete;
  RawSparse2D(const RawSparse2D &&that) = delete;
  RawSparse2D &operator=(RawSparse2D &that) = delete;
  RawSparse2D &operator=(RawSparse2D &&that) = delete;
  RawSparse2D &operator=(const RawSparse2D &that) = delete;
  RawSparse2D &operator=(const RawSparse2D &&that) = delete;
};

template <class Archive, class T>
bool load_sparse2dlist_with_raw_data(Archive &ar, std::vector<T> &data, std::vector<size_t> &info,
                                     std::vector<INDICE_TYPE> &indices, std::vector<INDICE_TYPE> &row_indices) {
  size_t rows = 0, cols = 0, size_sparse, size = 0;
  ar(size_sparse);
  ar(rows, cols, size);

  data.resize(data.size() + size_sparse);
  info.resize(info.size() + 5);
  indices.resize(indices.size() + size_sparse);
  row_indices.resize(row_indices.size() + rows + 1);

  T *s_data = &data[data.size()] - size_sparse;
  size_t *s_info = &info[info.size()] - 5;
  INDICE_TYPE *s_indices = &indices[indices.size()] - size_sparse;
  INDICE_TYPE *s_row_indices = &row_indices[row_indices.size()] - (rows + 1);

  ar(cereal::binary_data(s_data, sizeof(T) * size_sparse));
  ar(cereal::binary_data(s_indices, sizeof(INDICE_TYPE) * size_sparse));
  ar(cereal::binary_data(s_row_indices, sizeof(INDICE_TYPE) * (rows + 1)));

  s_info[0] = cols;
  s_info[1] = rows;
  s_info[2] = size_sparse;
  s_info[3] = data.size() - size_sparse;
  s_info[4] = row_indices.size() - (rows + 1);
  return true;
}

template <class T>
class Sparse2DList {
 private:
  static constexpr size_t INFO_SIZE = 5;
  static bool FROM_FILE(Sparse2DList &list, const std::string &&file) {
    std::ifstream bin_data(file, std::ios::in | std::ios::binary);
    cereal::PortableBinaryInputArchive iarchive(bin_data);
    return tick::load_sparse2dlist_with_raw_data(iarchive, list.m_data, list.m_info, list.m_indices,
                                                 list.m_row_indices);
  }

 public:
  Sparse2DList() {}
  RawSparse2D<T> operator[](size_t i) const {
    return RawSparse2D<T>(m_data.data() + (m_info[(i * INFO_SIZE) + 3]), &m_info[i * INFO_SIZE],
                          m_indices.data() + (m_info[(i * INFO_SIZE) + 3]),
                          m_row_indices.data() + (m_info[(i * INFO_SIZE) + 4]));
  }

  const INDICE_TYPE *indices() const { return m_indices.data(); }
  const INDICE_TYPE *row_indices() const { return m_row_indices.data(); }
  const size_t *info() const { return m_info.data(); }

  bool add_cereal(const std::string &file) { return FROM_FILE(*this, file); }

  static std::shared_ptr<Sparse2DList<T>> FROM_CEREALS(const std::vector<std::string> &&files) {
    auto array = std::make_shared<Sparse2DList<T>>();
    for (const auto &file : files)
      if (!FROM_FILE(*array.get(), file)) return nullptr;
    return std::move(array);
  }

 private:
  std::vector<T> m_data;
  std::vector<size_t> m_info;
  std::vector<INDICE_TYPE> m_indices, m_row_indices;

  Sparse2DList() = delete;
  Sparse2DList(Sparse2DList &that) = delete;
  Sparse2DList(const Sparse2DList &that) = delete;
  Sparse2DList(Sparse2DList &&that) = delete;
  Sparse2DList(const Sparse2DList &&that) = delete;
  Sparse2DList &operator=(Sparse2DList &that) = delete;
  Sparse2DList &operator=(Sparse2DList &&that) = delete;
  Sparse2DList &operator=(const Sparse2DList &that) = delete;
  Sparse2DList &operator=(const Sparse2DList &&that) = delete;
};

template <class T>
class RawSparse2DList {
 private:
  static constexpr size_t INFO_SIZE = 5;

 public:
  RawSparse2DList(std::vector<T> &data, std::vector<size_t> &info, std::vector<INDICE_TYPE> &_indices,
                  std::vector<INDICE_TYPE> &_rows_indices)
      : v_data(data.data()), v_info(info.data()), v_indices(_indices.data()), v_row_indices(_rows_indices.data()) {}

  RawSparse2D<T> operator[](size_t i) const {
    return RawSparse2D<T>(v_data + (v_info[(i * INFO_SIZE) + 3]), &v_info[i * INFO_SIZE],
                          v_indices + (v_info[(i * INFO_SIZE) + 3]), v_row_indices + (v_info[(i * INFO_SIZE) + 4]));
  }

  const INDICE_TYPE *indices() const { return v_indices; }
  const INDICE_TYPE *row_indices() const { return v_row_indices; }
  const size_t *info() const { return v_info; }

 private:
  T *v_data;
  size_t *v_info;
  INDICE_TYPE *v_indices, *v_row_indices;

  RawSparse2DList() = delete;
  RawSparse2DList(RawSparse2DList &that) = delete;
  RawSparse2DList(const RawSparse2DList &that) = delete;
  RawSparse2DList(RawSparse2DList &&that) = delete;
  RawSparse2DList(const RawSparse2DList &&that) = delete;
  RawSparse2DList &operator=(RawSparse2DList &that) = delete;
  RawSparse2DList &operator=(RawSparse2DList &&that) = delete;
  RawSparse2DList &operator=(const RawSparse2DList &that) = delete;
  RawSparse2DList &operator=(const RawSparse2DList &&that) = delete;
};

}  // namespace tick

#endif  //  TICK_ARRAY_ARRAY_HPP_
