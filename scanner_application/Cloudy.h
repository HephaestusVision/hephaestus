#ifndef CLOUDY_H
#define CLOUDY_H

class PointCloud;
class Cloudy {
public:
  Cloudy(/* FIXME: constructor arguments? */);
  virtual ~Cloudy();
  PointCloud const * GetCurrentPointCloud() const;
  void UpdatePointCloud();
  void CreatePointCloud();
  void ClearPointCloud();
private:
  Cloudy(const Cloudy & c); // noncopyable resource
  Cloudy& operator=(const Cloudy & c); // noncopyable resource
  /* FIXME: private instance variables */
};

#endif /* CLOUDY_H */
