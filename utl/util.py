import pickle
from pathlib import Path
import numpy as np

def convert_data_for_cpp(directory):
  """
  This function loads the data and stores them in a way to make loading in c++ more easy.
  Results are stored in a sub directory.

  Parameters
  ----------
  directory : path as string
    The path (including the dataset sub directory) where the files are present
  """

  # Each dataset gets subdirectory with the files for the c++ code
  directory = Path(directory)
  directory_cpp = directory

  def load_sparse_matrix(directory, name):
    """
    Loads all the components of a sparse matrix.

    Parameters
    ----------
    directory : Path
      The path where the files are present

    name : str
      The base name of the file

    Returns
    -------
    data : tuple
      The four components of a sparse matrix in CSR Format
    """

    indptr = np.load(directory / f"{name}_indptr.npy")
    indices = np.load(directory / f"{name}_indices.npy")
    data = np.load(directory / f"{name}_data.npy")
    shape = np.load(directory / f"{name}_shape.npy")

    return data, indices, indptr, shape

  def write_sparse_binary(filename):
    """ Wrapper to write sparse matrices in binary format """

    data, indices, indptr, shape = load_sparse_matrix(directory, filename)

    info_dict = {"rows":    shape[0],
                 "cols":    shape[1],
                 "data":    data.size,
                 "indices": indices.size,
                 "indptr":  indptr.size}

    return info_dict

  with open(directory / "parameters.pickle", "rb") as the_file:
    params = pickle.load(the_file)

  params["P-info"] = write_sparse_binary("P")

  with open(directory_cpp / "params.txt", "w") as the_file:
    the_file.write(f"{params['confusion_distance']}\n")
    the_file.write(f"{params['fuel_capacity']}\n")
    the_file.write(f"{params['max_controls']}\n")
    the_file.write(f"{params['number_stars']}\n")
    the_file.write(f"{params['NS']}\n")
    the_file.write(f"{params['P-info']['cols']}\n")
    the_file.write(f"{params['P-info']['rows']}\n")
    the_file.write(f"{params['P-info']['data']}\n")
    the_file.write(f"{params['P-info']['indices']}\n")
    the_file.write(f"{params['P-info']['indptr']}\n")

  return
