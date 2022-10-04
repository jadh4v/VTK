/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright Nonice for more information.

=========================================================================*/
/**
 * @class   vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor
 * @brief   Objects for traversal a HyperTreeGrid.
 *
 * JB A REVOIR
 * NonOriented ne peut pas remonter plus haut qu'a sa creation.
 * Objects that can perform depth traversal of a hyper tree grid,
 * take into account more parameters (related to the grid structure) than
 * the compact hyper tree cursor implemented in vtkHyperTree can.
 * This is an abstract class.
 * Cursors are created by the HyperTreeGrid implementation.
 *
 * Geometry cursors allow to retrieve origin, size, bounds
 * and central points
 *
 * @sa
 * vtkHyperTreeCursor vtkHyperTree vtkHyperTreeGrid
 *
 * @par Thanks:
 * This class was written by Guenole Harel and Jacques-Bernard Lekien, 2014.
 * This class was re-written by Philippe Pebay, 2016.
 * JB This class was re-written for more optimisation by Jacques-Bernard Lekien,
 * Guenole Harel and Jerome Dubois, 2018.
 * This work was supported by Commissariat a l'Energie Atomique
 * CEA, DAM, DIF, F-91297 Arpajon, France.
 */

#ifndef vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor_h
#define vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkObject.h"

#include "vtkHyperTreeGridGeometryUnlimitedEntry.h" // cache for historic
#include "vtkHyperTreeGridTools.h"                  // for HasTree
#include "vtkSmartPointer.h"                        // Used internally
#include <memory>                                   // std::shared_ptr
#include <vector>                                   // std::vector

VTK_ABI_NAMESPACE_BEGIN
class vtkHyperTree;
class vtkHyperTreeGrid;
class vtkHyperTreeGridScales;
class vtkHyperTreeGridOrientedGeometryCursor;
class vtkHyperTreeGridNonOrientedGeometryCursor;

class VTKCOMMONDATAMODEL_EXPORT vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor
  : public vtkObject
{
public:
  vtkTypeMacro(vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor* New();

  void Dump(ostream& os);

  // JB TODO 102018 On autorise le ToParent que jusqu'à ce que Level soit celui de la creation...
  // mais sans toRoot ? Une variante... qui serait utile aussi au niveau des SC

  /**
   * Create a copy of `this'.
   * \post results_exists:result!=0
   */
  virtual vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor* Clone();

  /**
   * Initialize cursor at root of given tree index in grid.
   */
  void Initialize(vtkHyperTreeGrid* grid, vtkIdType treeIndex, bool create = false);

  /**
   * Initialize cursor at root of given tree index in grid.
   */
  void Initialize(vtkHyperTreeGrid* grid, vtkHyperTree* tree, unsigned int level,
    vtkHyperTreeGridGeometryUnlimitedEntry& entry);

  /**
   * JB
   */
  void Initialize(vtkHyperTreeGrid* grid, vtkHyperTree* tree, unsigned int level, vtkIdType index,
    double* origin);

  /**
   * JB
   */
  void Initialize(vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor* cursor);

  ///@{
  /**
   * Return if a Tree pointing exist
   */
  bool HasTree() const { return vtk::hypertreegrid::HasTree(*this); }
  ///@}

  ///@{
  /**
   * Set the hyper tree to which the cursor is pointing.
   */
  vtkHyperTree* GetTree() const { return this->Tree; }
  ///@}

  /**
   * Return the index of the current vertex in the tree.
   */
  vtkIdType GetVertexId();

  /**
   * Return the global index (relative to the grid) of the
   * current vertex in the tree.
   */
  vtkIdType GetGlobalNodeIndex();

  /**
   * Return the dimension of the tree.
   * \post positive_result: result>0
   */
  unsigned char GetDimension();

  /**
   * Return the number of children for each node (non-vertex leaf) of the tree.
   * \post positive_number: result>0
   */
  unsigned char GetNumberOfChildren();

  /**
   * JB
   */
  void SetGlobalIndexStart(vtkIdType index);

  /**
   * JB
   */
  void SetGlobalIndexFromLocal(vtkIdType index);

  /**
   * JB
   */
  double* GetOrigin();
  double* GetSize();

  /**
   * JB
   */
  void GetBounds(double bounds[6]);
  void GetPoint(double point[3]);

  /**
   * Set the blanking mask is empty or not
   * \pre not_tree: tree
   */
  void SetMask(bool state);

  /**
   * Determine whether blanking mask is empty or not
   */
  bool IsMasked();

  /**
   * Is the cursor pointing to a leaf?
   * only respect depth limited, otherwise return false
   */
  bool IsLeaf();

  /**
   * Is the cursor pointing to a leaf in the original tree ?
   * Return false if the leaf is virtual.
   */
  bool IsRealLeaf();

  /**
   * Is the cursor pointing to a subdivided leaf ?
   * Return false if the leaf is a real one.
   */
  bool IsVirtualLeaf();

  /**
   * returns the value of the ratio to be applied, by multiplication, to
   * an extensive value (i.e. scaling with volume in 3D or surface in 2D)
   * betwee the center cursor and the given neighbors.
   * For intensive valued fields this ratio should not be used.
   */
  double GetExtensivePropertyRatio(vtkIdType index);

  /**
   * Is the cursor at tree root?
   */
  bool IsRoot();

  /**
   * Get the level of the tree vertex pointed by the cursor.
   */
  unsigned int GetLevel();

  /**
   * Move the cursor to child `child' of the current vertex.
   * \pre not_tree: HasTree()
   * \pre not_leaf: !IsLeaf()
   * \pre valid_child: ichild>=0 && ichild<GetNumberOfChildren()
   * \pre depth_limiter: GetLevel() <= GetDepthLimiter()
   */
  void ToChild(unsigned char ichild);

  /**
   * Move the cursor to the root vertex.
   * \pre can be root
   * \post is_root: IsRoot()
   */
  void ToRoot();

  /**
   * Move the cursor to the parent of the current vertex.
   * Authorized if HasHistory return true.
   * \pre Non_root: !IsRoot()
   */
  void ToParent();

  /**
   * Create a vtkHyperTreeGridOrientedGeometryCursor from input grid and
   * current entry data
   */
  vtkSmartPointer<vtkHyperTreeGridOrientedGeometryCursor> GetHyperTreeGridOrientedGeometryCursor(
    vtkHyperTreeGrid* grid);

  /**
   * Create a vtkHyperTreeGridNonOrientedGeometryCursor from input grid and
   * current entry data
   */
  vtkSmartPointer<vtkHyperTreeGridNonOrientedGeometryCursor>
  GetHyperTreeGridNonOrientedGeometryCursor(vtkHyperTreeGrid* grid);

protected:
  /**
   * Constructor
   * Only for vtkHyperTreeGridNonOrientedVonNeumannSuperCursor & Moore
   */
  vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor();

  /**
   * Destructor
   * Only for vtkHyperTreeGridNonOrientedVonNeumannSuperCursor & Moore
   */
  ~vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor() override;

  /**
   * Reference sur l'hyper tree grid parcouru actuellement.
   */
  vtkHyperTreeGrid* Grid;

  /**
   * JB
   */
  vtkHyperTree* Tree;

  /**
   * Storage of pre-computed per-level cell scales
   */
  std::shared_ptr<vtkHyperTreeGridScales> Scales;

  /**
   * JB
   */
  unsigned int Level;

  /**
   * Id of the last non-virtual entry
   */
  int LastValidEntry;

  // Hyper tree grid to which the cursor is attached
  std::vector<vtkHyperTreeGridGeometryUnlimitedEntry> Entries;

private:
  vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor(
    const vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor&) = delete;
  void operator=(const vtkHyperTreeGridNonOrientedUnlimitedGeometryCursor&) = delete;
};
VTK_ABI_NAMESPACE_END
#endif
