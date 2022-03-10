import { Component, OnInit, ChangeDetectionStrategy } from '@angular/core';

@Component({
  selector: 'app-tutorial-dialog',
  templateUrl: './tutorial-dialog.component.html',
  styleUrls: ['./tutorial-dialog.component.scss'],
  changeDetection: ChangeDetectionStrategy.OnPush
})
export class TutorialDialogComponent implements OnInit {

  constructor() { }

  ngOnInit(): void {
  }

}
