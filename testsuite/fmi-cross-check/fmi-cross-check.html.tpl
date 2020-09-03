<!DOCTYPE html>
<html>
  <head>
    <title>Fmi Cross Check test using OMSimulator</title>
  </head>
  <body>
    <h1>Fmi Cross Check test using OMSimulator</h1>

    <table>
      <tr>
        <th>Total</th>
        <th>Simulation</th>
        <th>Verification</th>
      </tr>
      <tr>
        <td>#Total#</td>
        <td bgcolor="#SimulationColor#">#Simulation#</td>
        <td bgcolor="#VerificationColor#">#Verification#</td>
      </tr>
    </table>

    <p>Total time taken: #totalTime# min:sec</p>
    <p>System info: #sysInfo#</p>
    <p>OMSimulator Version: #omsVersion#</p>
    <p>Test started: #timeStart#</p>
    <p>Tested Library: <a href="https://github.com/modelica/fmi-cross-check">fmi-cross-check</a> commit <a href="https://github.com/modelica/fmi-cross-check/commit/#commitfull#">#commitshort#</a></p>
    <p>Simulate FMU time limit: #ulimitOMSimulator#s</p>
    <p>Default tolerance: #default_tolerance#</p>
    Verified using: #referenceTool#
    <p>Links are provided if OMSimulator or the reference tool generates output.</p>
    <table id="resultsTable">
      <tr>
        <th onclick="sortTable(0)">FMI Version</th>
        <th onclick="sortTable(1)">FMI Type</th>
        <th onclick="sortTable(2)">Exporting Tool</th>
        <th onclick="sortTable(3)">Exporting Tool Version</th>
        <th onclick="sortTable(4)">Model</th>
        <th onclick="sortTable(5)">Verified</th>
        <th onclick="sortTable(6)">Simulated</th>
        <th onclick="sortTable(7)">Simulation Time [sec]</th>
      </tr>
      #testsHTML#
    </table>

    <!--
      "Borrowed" from: https://www.w3schools.com/howto/howto_js_sort_table.asp
      I'll argue that we use it for research reasons. You know I'm something of a scientist myself.
      From https://www.w3schools.com/about/about_copyright.asp:
      > Fair Use
      >   Fair use defines Refsnes Data as' exceptions to the rights protected by copyright laws.
      >   
      >   Fair use of W3Schools includes using copyrighted material:
      >   
      >   In research
      >   In news reporting
      >   In citations
      >   In commentary
    -->
    <script>
      function sortTable(n) {
        var table, rows, switching, i, x, y, shouldSwitch, dir, switchcount = 0;
        table = document.getElementById("resultsTable");
        switching = true;
        // Set the sorting direction to ascending:
        dir = "asc";
        /* Make a loop that will continue until
        no switching has been done: */
        while (switching) {
          // Start by saying: no switching is done:
          switching = false;
          rows = table.rows;
          /* Loop through all table rows (except the
          first, which contains table headers): */
          for (i = 1; i < (rows.length - 1); i++) {
            // Start by saying there should be no switching:
            shouldSwitch = false;
            /* Get the two elements you want to compare,
            one from current row and one from the next: */
            x = rows[i].getElementsByTagName("TD")[n];
            y = rows[i + 1].getElementsByTagName("TD")[n];
            /* Check if the two rows should switch place,
            based on the direction, asc or desc: */
            if (dir == "asc") {
              if (x.innerHTML.toLowerCase() > y.innerHTML.toLowerCase()) {
                // If so, mark as a switch and break the loop:
                shouldSwitch = true;
                break;
              }
            } else if (dir == "desc") {
              if (x.innerHTML.toLowerCase() < y.innerHTML.toLowerCase()) {
                // If so, mark as a switch and break the loop:
                shouldSwitch = true;
                break;
              }
            }
          }
          if (shouldSwitch) {
            /* If a switch has been marked, make the switch
            and mark that a switch has been done: */
            rows[i].parentNode.insertBefore(rows[i + 1], rows[i]);
            switching = true;
            // Each time a switch is done, increase this count by 1:
            switchcount ++;
          } else {
            /* If no switching has been done AND the direction is "asc",
            set the direction to "desc" and run the while loop again. */
            if (switchcount == 0 && dir == "asc") {
              dir = "desc";
              switching = true;
            }
          }
        }
      }
      </script>

  </body>
</html>
