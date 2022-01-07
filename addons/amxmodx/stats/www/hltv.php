<?php
include("pre.php");
//
$files = (new Demos)->get('./demos/');
?>
<?php if(!empty($files)): ?>
<div class="row">
    <div class="col-md-12">
        <table class="table table-sm table-striped table-bordered text-center table-dark small">
            <tr>
                <th>Name</th>
                <th>Date</th>
                <th>Map</th>
                <th>Download</th>
            </tr>
            <?php
            foreach($files as $row)
            {
                ?>
                <tr>
                    <td><?= $row['prefix'] ?></td>
                    <td><?= $row['map'] ?></td>
                    <td><?= $row['date'] ?></td>
                    <td><a class="text-reset" href="demos/<?= $row['file'] ?>"><?= $row['file'] ?></a></td>
                </tr>
                <?php
            }
            ?>
        </table>
    </div>
</div>
<?php else:?>
<div class="row mt-3">
    <div class="col-md-12">
        <h3 class="text-center text-white">No records found.</h3>
    </div>
</div>
<?php endif; ?>
<?php
include("pro.php");