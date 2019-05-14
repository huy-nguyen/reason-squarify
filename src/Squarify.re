// input data type representing the data contained in a leaf node (label +
// numerical value):
type datum = {
  value: float,
  label: string,
};

// Representation of a rectangle in browser-like coordinate system:
type rectangle = {
  /* Top-left corner: */
  x0: float,
  y0: float,
  /* Bottom right corner: */
  x1: float,
  y1: float,
};

// output data type:
type cell = {
  value: float,
  label: string,
  layout: rectangle,
};
// Type representing input data structure:
type tree =
  | LeafNode(datum)
  | BranchNode(list(tree));

let reducer = (accumulator, nextListElement) =>
  accumulator +. nextListElement;
// Returns the sum of a list of floats:
let sum = List.fold_left(reducer, 0.0);

[@bs.scope "Math"] [@bs.val] external abs: float => float = "";
// Returns the shorter side of a rectangle:
let getShorterSide = (rectangle: rectangle) => {
  let width = abs(rectangle.x1 -. rectangle.x0);
  let height = abs(rectangle.y1 -. rectangle.y0);
  width < height ? width : height;
};

// Sum up the numerical values contained in all leaf nodes
// at all levels of a given tree:
let rec sumValuesInTree = (node: tree): float =>
  switch (node) {
  // If this is a leaf node, we extract the numerical value from that node
  // through destructuring:
  | LeafNode(datum) => datum.value
  // If this is a branch node, we sum up the numerical values from all the sub
  // trees:
  | BranchNode(subTrees) =>
    // Note: recursive call:
    let valuesFromSubTrees = List.map(sumValuesInTree, subTrees);
    sum(valuesFromSubTrees);
  };

// max function that can handle either empty or non-empty arrays:
let rec generalizedMax = (input: list(float)) =>
  // First figure out whether the input is an empty list or not:
  switch (input) {
  // If input is empty, return an empty box (`None`):
  | [] => None
  // Otherwise, compare the `head` element of the list against the maximum value
  // among the remaining elements:
  | [head, ...tail] =>
    let boxContainingMaxOfRest = generalizedMax(tail);
    // Remember, `max` returns an `option` so we have to check whether the box
    // (`boxContainingMaxOfRest`) is empty or not:
    switch (boxContainingMaxOfRest) {
    // If the `boxContainingMaxOfRest` is empty, this means `head` is the only
    // element in the list so we can confidently return `head` as the max value
    // of the list:
    | None => Some(head)
    // Otherwise, compare `head` against the value in the box
    // (`numericMaxOfRest`). Note that although `numericMaxOfAll` is an actual
    // number, we must put it back in a box (`Some`) to preserve the API of
    // `max`, that is, it always returns a box:
    | Some(numericMaxOfRest) =>
      let numericMaxOfAll = head > numericMaxOfRest ? head : numericMaxOfRest;
      Some(numericMaxOfAll);
    };
  };

// max function whose input list is guaranteed to not be empty (hence expressed
// as a `head` of the list and the `rest` of the list)
let rec max = (head: float, rest: list(float)) =>
  switch (rest) {
  | [] => head
  | [headOfRest, ...restOfRest] =>
    let maxOfRest = max(headOfRest, restOfRest);
    head > maxOfRest ? head : maxOfRest;
  };

let rec min = (head: float, rest: list(float)) =>
  switch (rest) {
  | [] => head
  | [headOfRest, ...restOfRest] =>
    let minOfRest = min(headOfRest, restOfRest);
    head < minOfRest ? head : minOfRest;
  };

// Given a list of values of nodes contained in a row (split into `headOfRow`
// and `restOfRow`) and the length of the shorter side of a container, return
// the maximum aspect ratio among the rectangles contained in that row.
let getMaxAspectRatioInRow =
    (headOfRow: float, restOfRow: list(float), w: float): float => {
  // `rMinus` is the minimum value in the row:
  let rMinus = min(headOfRow, restOfRow);

  // `rPlus` is the maximum value in the row:
  let rPlus = max(headOfRow, restOfRow);

  // `s` is the sum of all values in the row
  let s = sum([headOfRow, ...restOfRow]);

  // the left and right operands of the "max" function in the formula:
  let maxOperand1 = w *. w *. rPlus /. (s *. s);
  let maxOperand2 = s *. s /. (w *. w *. rMinus);

  // Perform the "max" operation:
  maxOperand1 > maxOperand2 ? maxOperand1 : maxOperand2;
};

// Given a list of values of nodes and a container, assign each node a position
// (in the form of a rectangle) and return the remaining unfilled part of the
// container:
let fitRowIntoContainer = (row: list((_, float)), container: rectangle) => {
  let width = abs(container.x1 -. container.x0);
  let height = abs(container.y1 -. container.y0);
  let rowValues = List.map(((_, value)) => value, row);
  let rowAreaSum = sum(rowValues);
  let containerArea = width *. height;

  // How much of the container's area is occupied by the row
  let occupiedFraction = rowAreaSum /. containerArea;

  if (width <= height) {
    // If container is taller than it is wide (or square), do the layout
    // vertically (this branch). Otherwise, do it vertically (the else branch).

    // This is the horizontal divider line between the portion that'll contain
    // the nodes and the portion that remains after this layout step:
    let newY0 = container.y1 -. occupiedFraction *. height;
    // This is the empty portion of the container that remains after the nodes
    // in the row have been laid out:
    let remainingRectangle = {
      x0: container.x0,
      y0: container.y0,
      x1: container.x1,
      y1: newY0,
    };
    // Lay out the nodes vertically along the shorter side of the container:

    let (nodesWithLayout, _) =
      List.fold_left(
        (accumulator, nextNodeWithValue) => {
          let (result, prevX1) = accumulator;
          let (nextNode, nextNodeValue) = nextNodeWithValue;
          let nextWidth = nextNodeValue /. rowAreaSum *. width;
          let nextX1 = prevX1 +. nextWidth;
          let nextRectangle = {
            x0: prevX1,
            y0: newY0,
            x1: nextX1,
            y1: container.y1,
          };
          let nextAccumulator = (
            [(nextNode, nextRectangle), ...result],
            nextX1,
          );
          nextAccumulator;
        },
        ([], container.x0),
        row,
      );
    (nodesWithLayout, remainingRectangle);
  } else {
    // Lay out the nodes vertically:
    let newX1 = container.x0 +. occupiedFraction *. width;
    let remainingRectangle = {
      x0: newX1,
      y0: container.y0,
      x1: container.x1,
      y1: container.y1,
    };
    let (nodesWithLayout, _) =
      List.fold_left(
        (accumulator, nextNodeWithValue) => {
          let (result, prevY0) = accumulator;
          let (nextNode, nextNodeValue) = nextNodeWithValue;
          let nextHeight = nextNodeValue /. rowAreaSum *. height;
          let nextY0 = prevY0 -. nextHeight;
          let nextRectangle = {
            x0: container.x0,
            y0: nextY0,
            x1: newX1,
            y1: prevY0,
          };
          let nextAccumulator = (
            [(nextNode, nextRectangle), ...result],
            nextY0,
          );
          nextAccumulator;
        },
        ([], container.y1),
        row,
      );
    (nodesWithLayout, remainingRectangle);
  };
};

// Perform layout within a single level of the tree
let rec squarify =
        (
          // list of nodes that haven't been processed by the algorithm:
          remainingNodes: list(tree),
          // list of nodes that have been accepted into the current row but not
          // yet assigned a position in the layout:
          currentRow: list(tree),
          // current space available for layout:
          container: rectangle,
          // list of nodes that have already been assigned a layout position:
          result: list((tree, rectangle)),
        ) => {
  switch (remainingNodes, currentRow) {
  // Case 1: This is the base case. We're done because there are no more nodes
  // to process and the current row is empty.
  | ([], []) => result

  // Case 2: there are no more nodes to lay out but the last row has not been
  // added to the result so we add that last row to the result. This case is the
  // penultimate step in the algorithm before the base case is reached:
  | ([], [headOfCurrentRow, ...restOfCurrentRow]) =>
    let nodesInCurrentRowWithValues =
      List.map(
        node => (node, sumValuesInTree(node)),
        [headOfCurrentRow, ...restOfCurrentRow],
      );
    let (nodesWithLayout, remainingRectangle) =
      fitRowIntoContainer(nodesInCurrentRowWithValues, container);
    squarify(
      [],
      [],
      remainingRectangle,
      List.concat([nodesWithLayout, result]),
    );

  // Case 3: This means we're starting a new (empty) row. The only logical thing
  // to do here is put the the first remaining node into the new row. The next
  // step will fall into case 4:
  | ([headOfRemainingNodes, ...restOfRemainingNodes], []) =>
    squarify(restOfRemainingNodes, [headOfRemainingNodes], container, result)

  // Case 4: most general case.
  | (
      [headOfRemainingNodes, ...restOfRemainingNodes],
      [headOfCurrentRow, ...restOfCurrentRow],
    ) =>
    let shorterSide = getShorterSide(container);

    // Evaluate the current row's max aspect ratio:
    let valuesInCurrentRow = List.map(sumValuesInTree, restOfCurrentRow);
    let currentMaxAspectRatio =
      getMaxAspectRatioInRow(
        sumValuesInTree(headOfCurrentRow),
        valuesInCurrentRow,
        shorterSide,
      );

    // Evaluate the max aspect ratio if the next node is added to the current
    // row:
    let valuesInRowWithNextNodeAdded =
      List.map(sumValuesInTree, [headOfCurrentRow, ...restOfCurrentRow]);
    let maxAspectRatioWithNextNodeAdded =
      getMaxAspectRatioInRow(
        sumValuesInTree(headOfRemainingNodes),
        valuesInRowWithNextNodeAdded,
        shorterSide,
      );

    if (maxAspectRatioWithNextNodeAdded < currentMaxAspectRatio) {
      // This means we should add the next node to the current row because
      // it reduces the aspect ratio:

      // Add the next available node as the *last* element in the current row.
      // This is conceptually equivalent to
      // [headOfCurrentRow, ...restOfCurrentRow, headOfRemainingNodes]
      let currentRowWithNextNodeAdded =
        List.concat([
          [headOfCurrentRow, ...restOfCurrentRow],
          [headOfRemainingNodes],
        ]);
      // Then continue the layout process:
      squarify(
        restOfRemainingNodes,
        currentRowWithNextNodeAdded,
        container,
        result,
      );
    } else {
      // This means adding the next node to the current row will worsen the
      // aspect ratio. As such, we should assign rectangles to all nodes in the
      // `currentRow` and start a new row:

      let nodesInCurrentRowWithValues =
        List.map(
          node => (node, sumValuesInTree(node)),
          [headOfCurrentRow, ...restOfCurrentRow],
        );
      let (nodesWithLayout, remainingRectangle) =
        fitRowIntoContainer(nodesInCurrentRowWithValues, container);
      squarify(
        [headOfRemainingNodes, ...restOfRemainingNodes],
        [],
        remainingRectangle,
        List.concat([nodesWithLayout, result]),
      );
    };
  };
};

// Traverse the tree structure and perform the layout algorithm level-by-level
let rec traverse = (input: tree, container: rectangle): list(cell) =>
  switch (input) {
  // If this node is a leaf node, assign the rectangle directly to that node:
  | LeafNode({value, label}) => [{value, label, layout: container}]

  // If this node is a branch node, assign a rectangle to each of its branches,
  // then then descend into each branch to perform layout within each of those
  // rectangles. When done, flatten the resulting list:
  | BranchNode(subTrees) =>
    let nodesInCurrentLevelWithLayout = squarify(subTrees, [], container, []);
    let nodesInCurrentLevelAndBelowWithLayout =
      List.map(
        ((tree, innerContainer)) => traverse(tree, innerContainer),
        nodesInCurrentLevelWithLayout,
      );
    List.concat(nodesInCurrentLevelAndBelowWithLayout);
  };
